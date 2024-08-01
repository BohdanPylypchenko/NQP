#include "pch.h"

#include "nqp_mt_memory_map.h"

#include "nqp_mt.h"
#include "nqp_io.h"
#include "nqp_io_const.h"
#include "nqp_io_memory_map.h"
#include "file_buffer_adjust_const.h"
#include "write_concat.h"
#include "nqp_null_check.h"
#include "WinapiConfig.h"
#include "nqp_memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

static const char * const BASE_TMP_FILENAME = "out-";

unsigned long long nqp_mt_memory_map(int dim, int thread_count)
{
	HANDLE heap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
	nqp_null_check(heap);

	nqp_writer ** writer_arr = (nqp_writer **)HeapAlloc(heap, 0, dim * sizeof(nqp_writer *));
	nqp_null_check(writer_arr);

	nqp_init_args args;
	args.dim = dim;
	args.memory_size_in_bytes = get_available_physical_memory_bytes() / (4 * dim + 4);
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