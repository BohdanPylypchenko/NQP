#include "pch.h"

#include "nqp_mt_stdbuffer.h"

#include "nqp_mt.h"
#include "nqp_io.h"
#include "nqp_io_const.h"
#include "nqp_io_stdbuffer.h"
#include "file_buffer_adjust_const.h"
#include "write_concat.h"
#include "nqp_null_check.h"
#include "WinapiConfig.h"

#include <stdio.h>
#include <stdlib.h>

unsigned long long nqp_mt_stdbuffer(int dim, int thread_count)
{
	HANDLE heap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
	nqp_null_check(heap);

	nqp_writer ** writer_arr = (nqp_writer **)HeapAlloc(heap, 0, dim * sizeof(nqp_writer *));
	nqp_null_check(writer_arr);

	nqp_init_args args;
	args.dim = dim;
	args.bufsize_11 = BUFSIZE_11_SINGLE_FILE;
	args.bufsize_15 = BUFSIZE_15_SINGLE_FILE;
	args.bufsize_big = BUFSIZE_BIG_SINGLE_FILE;
	for (int i = 0; i < dim; i++)
	{
		args.id = i;

		writer_arr[i] = nqp_write_init(&args);
		if (writer_arr[i] == NULL)
		{
			fprintf(stderr, "Error: failed to initialize writer #%d\n", i);
			return -1;
		}
	}

	unsigned long long s_count = nqp_mt(dim, thread_count, writer_arr, NULL);

	for (int i = 0; i < dim; i++)
	{
		nqp_write_close(writer_arr[i]);
	}

	if (write_concat(dim, s_count) != 0)
	{
		fprintf(stderr, "Error: failed output concat\n");
	}

	HeapDestroy(heap);

	return s_count;
}