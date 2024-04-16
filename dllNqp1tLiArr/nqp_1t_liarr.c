#include "pch.h"

#include "nqp_1t_liarr.h"

#include "nqp_1t.h"
#include "nqp_io.h"
#include "nqp_io_const.h"
#include "nqp_io_liarr.h"
#include "file_buffer_adjust_const.h"
#include "nqp_dim_threadcount_constraint.h"
#include "liarr_adjust.h"
#include "liarr_adjust_const.h"

__declspec(dllexport) unsigned long long nqp_1t_liarr(int dim)
{
	if (nqp_validate_dim(dim) != 0)
	{
		return -1;
	}

	nqp_init_args init_args =
	{
		.dim = dim,
		.solution_capacity_incr_coef = DEFAULT_SOLUTION_CAPACITY_INCR_COEF,
		.head_solution_capacity = adjust_capacity(dim, SOLUTION_CAPACITY_8, SOLUTION_CAPACITY_11, SOLUTION_CAPACITY_15, SOLUTION_CAPACITY_BIG)
	};
	nqp_liarr * writer = nqp_write_init(&init_args);

	nqp_start_args start_args =
	{
		.dim = dim,
		.out_filename = TERMINAL_OUTPUT_FILENAME,
		.liarr_arr = &writer,
		.liarr_count = 1,
		.bufsize_11 = BUFSIZE_11_SINGLE_FILE,
		.bufsize_15 = BUFSIZE_15_SINGLE_FILE,
		.bufsize_big = BUFSIZE_BIG_SINGLE_FILE
	};

	unsigned long long s_count = nqp_1t(dim, writer, &start_args);

	nqp_write_end();
	nqp_write_close(writer);

	return s_count;
}