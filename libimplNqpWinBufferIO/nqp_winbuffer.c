#include "pch.h"

#include "nqp_io_winbuffer.h"
#include "nqp_winbuffer.h"

#include "nqp_null_check.h"
#include "WinapiConfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const DWORD MAX_WRITE_SIZE = MAXDWORD;

static void nqp_winbuffer_copy(
	nqp_winbuffer * winbuffer,
	size_t write_size_in_bytes, int * solution
);

nqp_winbuffer * nqp_winbuffer_new(
	unsigned long long winbuffer_id,
	HANDLE file, HANDLE heap,
	size_t size_in_bytes
) {

	nqp_winbuffer * winbuffer = (nqp_winbuffer *)HeapAlloc(heap, 0, sizeof(nqp_winbuffer));
	nqp_null_check(winbuffer);

	winbuffer->id = winbuffer_id;
	winbuffer->file = file;
	winbuffer->heap = heap;
	winbuffer->size_in_bytes = size_in_bytes;
	winbuffer->count_of_used_bytes = 0;
	winbuffer->solution_count = 0;

	winbuffer->buffer = (char *)HeapAlloc(heap, 0, size_in_bytes * sizeof(char));
	nqp_null_check(winbuffer->buffer);

	winbuffer->write_ptr = winbuffer->buffer;

	return winbuffer;
}

void nqp_winbuffer_append(nqp_winbuffer * winbuffer, int dim, int * solution)
{
	size_t write_size_in_bytes = dim * sizeof(int);
	if (winbuffer->count_of_used_bytes + write_size_in_bytes < winbuffer->size_in_bytes)
	{
		nqp_winbuffer_copy(winbuffer, write_size_in_bytes, solution);
	}
	else
	{
		nqp_winbuffer_flush(winbuffer);
		winbuffer->write_ptr = winbuffer->buffer;
		winbuffer->count_of_used_bytes = 0;
		nqp_winbuffer_copy(winbuffer, write_size_in_bytes, solution);
	}
	winbuffer->solution_count++;
}

void nqp_winbuffer_flush(nqp_winbuffer * winbuffer)
{
	DWORD byte_count_written;
	if (winbuffer->count_of_used_bytes <= MAX_WRITE_SIZE)
	{
		WriteFile(
			winbuffer->file, winbuffer->buffer,
			(DWORD)(winbuffer->count_of_used_bytes), &byte_count_written,
			NULL
		);
	}
	else
	{
		size_t write_count_whole = winbuffer->count_of_used_bytes / MAX_WRITE_SIZE;
		DWORD write_size_left = winbuffer->count_of_used_bytes % MAX_WRITE_SIZE;
		char * temp = winbuffer->buffer;
		for (size_t i = 0; i < write_count_whole; i++)
		{
			WriteFile(
				winbuffer->file, temp,
				MAX_WRITE_SIZE, &byte_count_written,
				NULL
			);
			temp += byte_count_written;
		}
		WriteFile(
			winbuffer->file, temp,
			write_size_left, &byte_count_written,
			NULL
		);
	}
}

static void nqp_winbuffer_copy(
	nqp_winbuffer * winbuffer,
	size_t write_size_in_bytes, int * solution
) {
	memcpy(winbuffer->write_ptr, solution, write_size_in_bytes);
	winbuffer->write_ptr += write_size_in_bytes;
	winbuffer->count_of_used_bytes += write_size_in_bytes;
}