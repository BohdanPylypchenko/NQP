#include "pch.h"

#include "nqp_mt_buffer.h"

#include "nqp_mt.h"
#include "nqp_io.h"
#include "nqp_io_const.h"
#include "nqp_io_buffer.h"
#include "file_buffer_adjust_const.h"
#include "int_clen.h"
#include "write_concat.h"
#include "nqp_fail_alloc_check.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

static const char * const BASE_TMP_FILENAME = "out-";

unsigned long long nqp_mt_buffer(int dim, int thread_count)
{
	nqp_writer ** writer_arr = (nqp_writer **)malloc(dim * sizeof(nqp_writer *));
	nqp_fail_alloc_check(writer_arr);

	int base_tmp_filename_len = (int)strnlen_s(BASE_TMP_FILENAME, INT_MAX - 1);
	int nqp_extension_len = (int)strnlen_s(NQP_EXTENSION, INT_MAX - 1);
	nqp_init_args args;
	args.dim = dim;
	args.bufsize_11 = BUFSIZE_11_SINGLE_FILE;
	args.bufsize_15 = BUFSIZE_15_SINGLE_FILE;
	args.bufsize_big = BUFSIZE_BIG_SINGLE_FILE;
	for (int i = 0; i < dim; i++)
	{
		args.id = i;

		int int_clen = INT_CLEN(i + 1);
		int filename_len = base_tmp_filename_len + int_clen + nqp_extension_len + 1;
		args.out_filename = (char *)malloc(filename_len * sizeof(char));
		nqp_fail_alloc_check(args.out_filename);
		sprintf_s(args.out_filename, filename_len * sizeof(char), "%s%d%s", BASE_TMP_FILENAME, i + 1, NQP_EXTENSION);
		args.out_filename[filename_len - 1] = '\0';

		writer_arr[i] = nqp_write_init(&args);
		if (writer_arr[i] == NULL)
		{
			fprintf(stderr, "Error: failed to initialize writer #%d\n", i);
			return -1;
		}
	}

	unsigned long long s_count = nqp_mt(dim, thread_count, writer_arr, NULL);

	char ** out_filename_arr = (char **)malloc(dim * sizeof(char *));
	nqp_fail_alloc_check(out_filename_arr);
	for (int i = 0; i < dim; i++)
	{
		out_filename_arr[i] = writer_arr[i]->out_filename;
		nqp_write_close(writer_arr[i]);
	}

	if (write_concat(dim, dim, out_filename_arr) != 0)
	{
		fprintf(stderr, "Error: failed output concat\n");
	}

	for (int i = 0; i < dim; i++)
	{
		free(out_filename_arr[i]);
	}
	free(writer_arr);

	return s_count;
}