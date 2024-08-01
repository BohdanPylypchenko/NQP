#include "pch.h"

#include "nqp_1t_winbuffer.h"

#include "nqp_1t.h"
#include "nqp_io.h"
#include "nqp_io_const.h"
#include "nqp_io_winbuffer.h"
#include "file_buffer_adjust_const.h"
#include "nqp_dim_threadcount_constraint.h"
#include "write_concat.h"

#include "nqp_memory.h"

__declspec(dllexport) unsigned long long nqp_1t_winbuffer(int dim)
{
	if (nqp_validate_dim(dim) != 0)
	{
		return -1;
	}

	nqp_init_args init_args =
	{
		.id = 0,
		.size_in_bytes = get_available_physical_memory_bytes() / 64,
		.dim = dim
	};
	nqp_winbuffer * writer = nqp_write_init(&init_args);

	unsigned long long s_count = nqp_1t(dim, writer, NULL);

	nqp_write_end();
	nqp_write_close(writer);

	write_concat(dim, s_count);

	return s_count;
}