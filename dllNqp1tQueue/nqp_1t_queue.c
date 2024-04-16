#include "pch.h"

#include "nqp_1t_queue.h"

#include "nqp_1t.h"
#include "nqp_io.h"
#include "nqp_io_const.h"
#include "nqp_io_queue.h"
#include "file_buffer_adjust_const.h"
#include "nqp_dim_threadcount_constraint.h"

__declspec(dllexport) unsigned long long nqp_1t_queue(int dim)
{
	if (nqp_validate_dim(dim) != 0)
	{
		return -1;
	}

	nqp_queue * writer = nqp_write_init(NULL);
	nqp_start_args start_args =
	{
		.dim = dim,
		.out_filename = TERMINAL_OUTPUT_FILENAME,
		.queue_arr = &writer,
		.queue_count = 1,
		.bufsize_11 = BUFSIZE_11_SINGLE_FILE,
		.bufsize_15 = BUFSIZE_15_SINGLE_FILE,
		.bufsize_big = BUFSIZE_BIG_SINGLE_FILE
	};

	unsigned long long s_count = nqp_1t(dim, writer, &start_args);

	nqp_write_end();
	nqp_write_close(writer);

	return s_count;
}