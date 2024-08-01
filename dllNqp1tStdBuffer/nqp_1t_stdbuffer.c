#include "pch.h"

#include "nqp_1t.h"
#include "nqp_io.h"
#include "nqp_io_const.h"
#include "nqp_io_stdbuffer.h"
#include "file_buffer_adjust_const.h"
#include "nqp_dim_threadcount_constraint.h"
#include "write_concat.h"

#include <stdio.h>
#include <stdlib.h>

__declspec(dllexport) unsigned long long nqp_1t_stdbuffer(int dim)
{
	if (nqp_validate_dim(dim) != 0)
	{
		return -1;
	}

	nqp_init_args args =
	{
		.id = 1,
		.dim = dim,
		.bufsize_11 = BUFSIZE_11_SINGLE_FILE,
		.bufsize_15 = BUFSIZE_15_SINGLE_FILE,
		.bufsize_big = BUFSIZE_BIG_SINGLE_FILE
	};
	nqp_writer * writer = nqp_write_init(&args);
	if (writer == NULL)
	{
		fprintf(stderr, "Error: failed to initialize writer\n");
		return -1;
	}

	unsigned long long s_count = nqp_1t(dim, writer, NULL);

	nqp_write_end();
	nqp_write_close(writer);

	if (write_concat(dim, s_count) != 0)
	{
		fprintf(stderr, "Error: failed output concat\n");
	}

	return s_count;
}
