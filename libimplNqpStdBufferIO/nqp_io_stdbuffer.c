#include "pch.h"

#include "nqp_io_stdbuffer.h"

#include "nqp_io.h"
#include "nqp_null_check.h"
#include "file_buffer_adjust.h"
#include "nqp_inject.h"
#include "WinapiConfig.h"
#include "nqp_create_temp_file_std.h"

#include <stdio.h>
#include <stdlib.h>

nqp_writer * nqp_write_init(nqp_init_args * args)
{
	HANDLE heap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
	nqp_null_check(heap);

	nqp_writer * writer = (nqp_writer *)HeapAlloc(heap, 0, sizeof(nqp_writer));
	nqp_null_check(writer);

	writer->heap = heap;
	writer->id = args->id;
	writer->solution_count = 0;
	
	FILE * f = create_temp_file_std(writer->id, 0);
	if (f == NULL)
	{
		return NULL;
	}

	writer->out = f;
	
	if (adjust_buf_size(writer->out, args->dim,
		args->bufsize_11, args->bufsize_15, args->bufsize_big) != 0)
	{
		fprintf(stderr, "Error: can't resize writer stdbuffer\n");
		fclose(writer->out);
		HeapDestroy(heap);
		return NULL;
	}

	if (inject_dim(args->dim, writer->out) != INJECT_DIM_SUCCESS)
	{
		fclose(writer->out);
		HeapDestroy(heap);
		return NULL;
	}

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
	size_t written_count = fwrite(queens, sizeof(int), dim, writer->out);
	writer->solution_count++;
	if (written_count != dim)
	{
		fprintf(stderr, "Error: failed to write solution %p by writer %lld\n", queens, writer->id);
	}
}

void nqp_write_close(nqp_writer * writer)
{
	if (inject_total_solution_count(writer->solution_count, writer->out) != INJECT_TOTAL_SOLUTION_COUNT_SUCCESS)
	{
		fprintf(stderr, "Error: can't inject total solution count into output stream %p\n", writer->out);
	}

	if (fflush(writer->out) != 0)
	{
		fprintf(stderr, "Error: can't flush output stream %p; Errno = %d\n", writer->out, errno);
	}

	if (fclose(writer->out) != 0)
	{
		fprintf(stderr, "Error when closing stream\n");
	}

	HANDLE heap = writer->heap;
	HeapDestroy(heap);
}