#include "pch.h"

#include "nqp_io.h"
#include "nqp_io_liarr.h"
#include "nqp_liarr.h"
#include "file_buffer_adjust.h"
#include "nqp_inject.h"

#include <stdlib.h>
#include <stdio.h>

static FILE * _out;
static int _dim;

static int _liarr_count;
static nqp_liarr ** _liarr_arr;

static unsigned long long total_solution_count;
static unsigned long long _fill;

nqp_writer * nqp_write_init(nqp_init_args * args)
{
	return nqp_liarr_new(
		args->solution_capacity_incr_coef,
		args->dim, args->head_solution_capacity
	);
}

int nqp_write_start(nqp_start_args * args)
{
	//printf("Start liarr nqp-io\n");

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

	_liarr_count = args->liarr_count;
	_liarr_arr = args->liarr_arr;

	_dim = args->dim;
	_fill = 0;

	return 0;
}

void nqp_write_notify_computation_complete(unsigned long long s_count)
{
	total_solution_count = s_count;
}

void nqp_write_wait()
{
	return;
}

void nqp_write_end()
{
	for (int i = 0; i < _liarr_count; i++)
	{
		nqp_liarr_element * current;
		unsigned long long int_count;
		while ((current = nqp_liarr_pop(_liarr_arr[i])) != NULL)
		{
			int_count = _dim * current->solution_fill;
			if (fwrite(current->solutions, sizeof(int), int_count, _out) != int_count)
				fprintf(stderr, "Error: failed to write solution\n");
			_fill += int_count;
			nqp_liarr_element_free(current, _liarr_arr[i]->heap);
		}

		nqp_liarr_free(_liarr_arr[i]);
	}
}

void nqp_write_solution(int dim, int * queens, nqp_writer * writer)
{
	nqp_liarr_append(writer, dim, queens);
}

void nqp_write_close(nqp_writer * writer)
{
	if (inject_total_solution_count(total_solution_count, _out) != INJECT_TOTAL_SOLUTION_COUNT_SUCCESS)
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
}