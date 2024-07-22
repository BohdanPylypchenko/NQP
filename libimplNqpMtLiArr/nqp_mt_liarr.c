#include "pch.h"

#include "nqp_mt_liarr.h"

#include "nqp_mt.h"
#include "nqp_io.h"
#include "nqp_io_const.h"
#include "nqp_io_liarr.h"
#include "file_buffer_adjust_const.h"
#include "liarr_adjust.h"
#include "liarr_adjust_const.h"
#include "nqp_null_check.h"
#include "WinapiConfig.h"

#include <stdio.h>
#include <stdlib.h>

unsigned long long nqp_mt_liarr(int dim, int thread_count)
{
	HANDLE heap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
	nqp_null_check(heap);

	nqp_init_args init_args =
	{
		.dim = dim,
		.solution_capacity_incr_coef = DEFAULT_SOLUTION_CAPACITY_INCR_COEF,
		.head_solution_capacity = adjust_capacity(dim, SOLUTION_CAPACITY_8, SOLUTION_CAPACITY_11, SOLUTION_CAPACITY_15, SOLUTION_CAPACITY_BIG)
	};

	nqp_writer ** writer_arr = (nqp_writer **)HeapAlloc(heap, 0, dim * sizeof(nqp_writer *));
	nqp_null_check(writer_arr);

	for (int i = 0; i < dim; i++)
	{
		writer_arr[i] = nqp_write_init(&init_args);
	}

	nqp_start_args start_args;
	start_args.dim = dim;
	start_args.liarr_count = dim;
	start_args.out_filename = TERMINAL_OUTPUT_FILENAME;
	start_args.liarr_arr = writer_arr;
	start_args.bufsize_11 = BUFSIZE_11_SINGLE_FILE;
	start_args.bufsize_15 = BUFSIZE_15_SINGLE_FILE;
	start_args.bufsize_big = BUFSIZE_BIG_SINGLE_FILE;

	unsigned long long s_count = nqp_mt(dim, thread_count, writer_arr, &start_args);

	nqp_write_close(NULL);

	HeapDestroy(heap);

	return s_count;
}