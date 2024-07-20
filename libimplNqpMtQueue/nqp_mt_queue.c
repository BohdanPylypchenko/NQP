#include "pch.h"

#include "nqp_mt_queue.h"

#include "nqp_mt.h"
#include "nqp_io.h"
#include "nqp_io_queue.h"
#include "nqp_queue.h"
#include "nqp_io_const.h"
#include "file_buffer_adjust_const.h"
#include "nqp_fail_alloc_check.h"
#include "WinapiConfig.h"

#include <stdio.h>
#include <stdlib.h>

unsigned long long nqp_mt_queue(int dim, int thread_count)
{
	HANDLE heap = HeapCreate(0, 0, 0);
	nqp_fail_alloc_check(heap);

	nqp_writer ** writer_arr = (nqp_writer **)HeapAlloc(heap, 0, dim * sizeof(nqp_writer *));
	nqp_fail_alloc_check(writer_arr);
	for (int i = 0; i < dim; i++)
	{
		writer_arr[i] = nqp_write_init(NULL);
	}
	nqp_start_args start_args;
	start_args.dim = dim;
	start_args.queue_count = dim;
	start_args.out_filename = TERMINAL_OUTPUT_FILENAME;
	start_args.queue_arr = writer_arr;
	start_args.bufsize_11 = BUFSIZE_11_SINGLE_FILE;
	start_args.bufsize_15 = BUFSIZE_15_SINGLE_FILE;
	start_args.bufsize_big = BUFSIZE_BIG_SINGLE_FILE;

	unsigned long long s_count = nqp_mt(dim, thread_count, writer_arr, &start_args);

	nqp_write_close(NULL);

	HeapDestroy(heap);

	return s_count;
}