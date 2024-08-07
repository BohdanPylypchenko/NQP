#include "pch.h"

#include "nqp_io.h"
#include "nqp_io_queue.h"
#include "nqp_queue.h"
#include "nqp_null_check.h"
#include "file_buffer_adjust.h"
#include "nqp_inject.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

static FILE * _out;

static int _queue_count;
static nqp_queue ** _queue_arr;

static int _is_computation_completed;
static unsigned long long _write_count;
static unsigned long long _max_write_count;

static HANDLE _writer_thread = NULL;

static DWORD WINAPI _nqp_ping_write_loop(LPVOID arg);
static int _check_to_continue();
static int _write_solution(nqp_queue_element * element);

nqp_writer * nqp_write_init(nqp_init_args * args)
{
	return nqp_queue_new();
}

int nqp_write_start(nqp_start_args * args)
{
	if (fopen_s(&_out, args->out_filename, "wb") != 0)
	{
		return 1;
	}

	if (adjust_buf_size(_out, args->dim,
		args->bufsize_11, args->bufsize_15, args->bufsize_big) != 0)
	{
		fclose(_out);
		return 1;
	}

	if (inject_dim(args->dim, _out) != INJECT_DIM_SUCCESS)
	{
		fclose(_out);
		return 1;
	}

	_queue_count = args->queue_count;
	_queue_arr = args->queue_arr;

	_is_computation_completed = 0;
	_write_count = 0;
	_max_write_count = LLONG_MAX;

	_writer_thread = CreateThread(
		NULL,                 // default security attributes
		0,                    // default stack size
		_nqp_ping_write_loop, // thread task
		NULL,                 // no argument for task
		0,                    // Run immediately
		NULL                  // don't safe thread identifier
	);
	if (_writer_thread == NULL)
	{
		fclose(_out);
		return 1;
	}

	return 0;
}

void nqp_write_notify_computation_complete(unsigned long long max_write_count)
{
	_max_write_count = max_write_count;
	_is_computation_completed = 1;
}

void nqp_write_wait()
{
	WaitForSingleObject(_writer_thread, INFINITE);
}

void nqp_write_end()
{
	for (int i = 0; i < _queue_count; i++)
		nqp_queue_free(_queue_arr[i]);
}

void nqp_write_solution(int dim, int * solution, nqp_writer * writer)
{
	int * solution_copy = (int *)HeapAlloc(writer->heap, 0, dim * sizeof(int));
	nqp_null_check(solution_copy);

	memcpy(solution_copy, solution, dim * sizeof(int));

	nqp_queue_element * element = nqp_queue_element_new(
		dim, solution_copy,
		writer->heap
	);

	nqp_queue_append(writer, element);
}

void nqp_write_close(nqp_writer * writer)
{
	if (inject_total_solution_count(_max_write_count, _out) != INJECT_TOTAL_SOLUTION_COUNT_SUCCESS)
	{
		fprintf(stderr, "Error: can't inject total solution count into output stream %p\n", _out);
	}

	if (fflush(_out) != 0)
	{
		fprintf(stderr, "Error: can't flush output stream %p; Errno = %d\n", _out, errno);
	}

	if (fclose(_out) != 0)
	{
		fprintf(stderr, "Error when closing stream\n");
	}

	if (!CloseHandle(_writer_thread))
	{
		fprintf(stderr, "Error while closing thread handle\n");
	}
}

static DWORD WINAPI _nqp_ping_write_loop(LPVOID arg)
{
	UNREFERENCED_PARAMETER(arg);

	for (unsigned long long i = 0; _check_to_continue(); i++)
	{
		nqp_queue * current_queue = _queue_arr[i % _queue_count];

		nqp_queue_element * element = nqp_queue_pop_safe(current_queue);

		if (element == NULL)
			continue;

		if (_write_solution(element) != 0)
			fprintf(stderr, "Error: failed to write solution #%llu\n", _write_count);

		nqp_queue_element_free(element, current_queue->heap);
		element = NULL;
	}

	for (int i = 0; i < _queue_count; i++)
	{
		nqp_queue_element * element;
		while ((element = nqp_queue_pop(_queue_arr[i])) != NULL)
		{
			if (_write_solution(element) != 0)
				fprintf(stderr, "Error: failed to write solution #%llu\n", _write_count);

			nqp_queue_element_free(element, _queue_arr[i]->heap);
			element = NULL;
		}
	}

	return 0;
}

static int _check_to_continue()
{
	if (!_is_computation_completed)
		return 1;
	else
		return 0;
}

static int _write_solution(nqp_queue_element * element)
{
	size_t w = fwrite(element->solution, sizeof(int), element->dim, _out);
	_write_count++;

	if (w == element->dim)
		return 0;
	else
		return 1;
}