#include "pch.h"

#include "nqp_io_buffer.h"

#include "nqp_memory_map.h"
#include "nqp_io.h"
#include "nqp_io_const.h"
#include "nqp_null_check.h"
#include "file_buffer_adjust.h"
#include "nqp_inject.h"
#include "WinapiConfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEMP_FILE_NAME_FORMAT L"tmp-%u-%u-%llu-%llu%s"
#define TEMP_FILE_NAME_LENGTH 128

//static int call_count = 0;

static HANDLE create_temp_file(
	unsigned long long writer_id,
	unsigned long long memory_map_id
);

static void finalize_writer(
	nqp_writer * writer
);

nqp_writer * nqp_write_init(nqp_init_args * args)
{
	HANDLE heap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
	nqp_null_check(heap);

	nqp_writer * writer = HeapAlloc(heap, 0, sizeof(nqp_writer));
	nqp_null_check(writer);

	unsigned long long initial_memory_map_id = 0;

	HANDLE file = create_temp_file(args->id, initial_memory_map_id);
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
	//printf("Start fwrite nqp-io\n");
	return 0;
}

void nqp_write_notify_computation_complete(unsigned long long s_count)
{
	//printf("Computation nqp completed; solution count = %lld\n", s_count);
	//total_solution_count = s_count;
}

void nqp_write_wait()
{
	//printf("Wait fwrite nqp-io\n");
}

void nqp_write_end()
{
	//printf("End fwrite nqp-io\n");
}

void nqp_write_solution(int dim, int * queens, nqp_writer * writer)
{
	//call_count++;

	size_t q_size = dim * sizeof(int);

	if (nqp_memory_map_try_write(
		writer->memory_map,
		(LPBYTE)queens, q_size) != NQP_MEMORY_MAP_SUCCESS)
	{
		finalize_writer(writer);

		writer->current_memory_map_id++;
		writer->solution_count = 0;

		writer->file = create_temp_file(
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

static HANDLE create_temp_file(
	unsigned long long writer_id,
	unsigned long long memory_map_id
) {
	WCHAR temp_file_name[TEMP_FILE_NAME_LENGTH];
	swprintf(
		temp_file_name, TEMP_FILE_NAME_LENGTH, TEMP_FILE_NAME_FORMAT,
	    GetCurrentProcessId(), GetCurrentThreadId(), writer_id, memory_map_id,
		NQP_TEMP_EXTENSION_W
	);

	HANDLE file = CreateFileW(
		temp_file_name,
		GENERIC_READ | GENERIC_WRITE,
		0, NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (file == INVALID_HANDLE_VALUE)
	{
		fwprintf(
			stderr, L"Error: can't create temp file %s; Error code = %d\n",
			temp_file_name, GetLastError()
		);
	}
	
	return file;
}

static void finalize_writer(
	nqp_writer * writer
) {
	memcpy(
		writer->memory_map->mbegin + sizeof(int),
		&writer->solution_count,
		sizeof(unsigned long long)
	);

	nqp_memory_map_free(writer->heap, writer->memory_map);

	CloseHandle(writer->file);
}