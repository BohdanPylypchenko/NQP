#include "pch.h"

#include "nqp_io_memory_map.h"

#include "nqp_memory_map.h"
#include "nqp_io.h"
#include "nqp_io_const.h"
#include "nqp_null_check.h"
#include "file_buffer_adjust.h"
#include "nqp_inject.h"
#include "nqp_create_temp_file_win.h"
#include "WinapiConfig.h"

#include <stdio.h>
#include <stdlib.h>

static void finalize_writer(nqp_writer * writer);

nqp_writer * nqp_write_init(nqp_init_args * args)
{
	HANDLE heap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
	nqp_null_check(heap);

	nqp_writer * writer = HeapAlloc(heap, 0, sizeof(nqp_writer));
	nqp_null_check(writer);

	unsigned long long initial_memory_map_id = 0;

	HANDLE file = create_temp_file_win(args->id, initial_memory_map_id);
	if (file == INVALID_HANDLE_VALUE)
	{
		HeapDestroy(heap);
		return NULL;
	}

	nqp_memory_map * memory_map = nqp_memory_map_new(
		initial_memory_map_id,
		heap, file,
		args->memory_size_in_bytes,
		args->dim
	);

	writer->id = args->id;
	writer->heap = heap;
	writer->file = file;
	writer->dim = args->dim;
	writer->solution_count = 0;
	writer->current_memory_map_id = initial_memory_map_id;
	writer->memory_size_in_bytes = args->memory_size_in_bytes;
	writer->memory_map = memory_map;

	return writer;
}

int nqp_write_start(nqp_start_args * nqp_start_args)
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
	size_t q_size = dim * sizeof(int);

	if (nqp_memory_map_try_write(
		writer->memory_map,
		(LPBYTE)queens, q_size) != NQP_MEMORY_MAP_SUCCESS)
	{
		finalize_writer(writer);

		writer->current_memory_map_id++;
		writer->solution_count = 0;

		writer->file = create_temp_file_win(
			writer->id,
			writer->current_memory_map_id
		);

		writer->memory_map = nqp_memory_map_new(
			writer->current_memory_map_id,
			writer->heap,
			writer->file,
			writer->memory_size_in_bytes,
			writer->dim
		);

		nqp_memory_map_try_write(
			writer->memory_map,
			(LPBYTE)queens, q_size
		);
	}

	writer->solution_count++;
}

void nqp_write_close(nqp_writer * writer)
{
	finalize_writer(writer);

	HANDLE heap = writer->heap;
	HeapDestroy(heap);
}

static void finalize_writer(nqp_writer * writer)
{
	memcpy(
		writer->memory_map->mbegin + sizeof(int),
		&writer->solution_count,
		sizeof(unsigned long long)
	);

	nqp_memory_map_free(writer->heap, writer->memory_map);

	CloseHandle(writer->file);
}