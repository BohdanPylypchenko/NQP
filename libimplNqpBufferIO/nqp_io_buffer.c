#include "pch.h"

#include "nqp_io_buffer.h"

#include "nqp_io.h"
#include "nqp_fail_alloc_check.h"
#include "file_buffer_adjust.h"
#include "WinapiConfig.h"

#include <stdio.h>
#include <stdlib.h>

nqp_writer * nqp_write_init(nqp_init_args * args)
{
	HANDLE heap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
	nqp_fail_alloc_check(heap);

	nqp_writer * writer = (nqp_writer *)HeapAlloc(heap, 0, sizeof(nqp_writer));
	nqp_fail_alloc_check(writer);

	writer->id = args->id;
	writer->out_filename = args->out_filename;
	errno_t error = fopen_s(&(writer->out), args->out_filename, "wb");
	if (error != 0)
	{
		fprintf(stderr, "Error: can't open file %s; Errno = %d\n", args->out_filename, error);
		HeapDestroy(heap);
		return NULL;
	}

	if (adjust_buf_size(writer->out, args->dim,
		args->bufsize_11, args->bufsize_15, args->bufsize_big) != 0)
	{
		fprintf(stderr, "Error: can't resize writer buffer\n");
		fclose(writer->out);
		HeapDestroy(heap);
		return NULL;
	}

	if (fwrite(&(args->dim), sizeof(int), 1, writer->out) != 1)
	{
		fclose(writer->out);
		HeapDestroy(heap);
		return NULL;
	}

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
	size_t written_count = fwrite(queens, sizeof(int), dim, writer->out);
	if (written_count != dim)
	{
		fprintf(stderr, "Error: failed to write solution %p by writer %lld\n", queens, writer->id);
	}
}

void nqp_write_close(nqp_writer * writer)
{
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