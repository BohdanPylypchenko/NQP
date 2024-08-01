#include "pch.h"

#include "nqp_io.h"
#include "nqp_io_winbuffer.h"
#include "nqp_winbuffer.h"
#include "file_buffer_adjust.h"
#include "nqp_inject.h"

#include <stdlib.h>
#include <stdio.h>

nqp_writer * nqp_write_init(nqp_init_args * args)
{
	HANDLE file = create_temp_file_win(args->id, 0);
	if (file == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	HANDLE heap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
	nqp_null_check(heap);

	DWORD byte_count_written;
	WriteFile(file, &args->dim, sizeof(int), &byte_count_written, NULL);
	unsigned long long zero = 0;
	WriteFile(file, &zero, sizeof(unsigned long long), &byte_count_written, NULL);

	return nqp_winbuffer_new(args->id, file, heap, args->size_in_bytes);
}

int nqp_write_start(nqp_start_args * args)
{
	return 0;
}

void nqp_write_notify_computation_complete(unsigned long long s_count)
{
	return;
}

void nqp_write_wait()
{
	return;
}

void nqp_write_end()
{
	return;
}

void nqp_write_solution(int dim, int * queens, nqp_writer * writer)
{
	nqp_winbuffer_append(writer, dim, queens);
}

void nqp_write_close(nqp_writer * writer)
{
	nqp_winbuffer_flush(writer);

	LARGE_INTEGER fptr; DWORD byte_count_written;
	fptr.QuadPart = 0 + sizeof(int);
	SetFilePointerEx(writer->file, fptr, NULL, FILE_BEGIN);
	WriteFile(
		writer->file, &writer->solution_count,
		sizeof(unsigned long long), &byte_count_written,
		NULL
	);

	CloseHandle(writer->file);

	HANDLE heap = writer->heap;
	HeapDestroy(heap);
}