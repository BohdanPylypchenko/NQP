#include "pch.h"

#include "write_concat.h"
#include "file_buffer_adjust.h"
#include "file_buffer_adjust_const.h"
#include "nqp_io_const.h"
#include "nqp_null_check.h"

#include <stdio.h>

#define TBUFSIZE 4096

int write_concat(
	int dim,
	unsigned long long total_solution_count,
	int file_count, char ** filename_arr
) {
	FILE * out;
	if (fopen_s(&out, TERMINAL_OUTPUT_FILENAME, "wb") != 0)
	{
		fprintf(stderr, "Error: can't open terminal file to concat solutions\n");
		return -1;
	}

	if (adjust_buf_size(out, dim,
		BUFSIZE_11_SINGLE_FILE, BUFSIZE_15_SINGLE_FILE, BUFSIZE_BIG_SINGLE_FILE) != 0)
	{
		fclose(out);
		fprintf(stderr, "Error: can't resize terminal file buffer size\n");
		return -1;
	}

	if (fwrite(&dim, sizeof(int), 1, out) != 1)
	{
		fclose(out);
		fprintf(stderr, "Error: can't write dim to terminal file\n");
		return -1;
	}

	if (fwrite(&total_solution_count, sizeof(unsigned long long), 1, out) != 1)
	{
		fclose(out);
		fprintf(stderr, "Error: can't write dim to terminal file\n");
		return -1;
	}

	FILE * tmp; char tbuf[TBUFSIZE];
	for (int i = 0; i < file_count; i++)
	{
		if (fopen_s(&tmp, filename_arr[i], "rb") != 0)
		{
			fclose(out);
			fprintf(stderr, "Error: can't open file %s\n", filename_arr[i]);
			return -1;
		}

		if (adjust_buf_size(out, dim,
			BUFSIZE_11_SINGLE_FILE, BUFSIZE_15_SINGLE_FILE, BUFSIZE_BIG_SINGLE_FILE) != 0)
		{
			fclose(out);
			fclose(tmp);
			fprintf(stderr, "Error: can't resize buffer of file %s\n", filename_arr[i]);
			return -1;
		}

		fseek(tmp, sizeof(int) + sizeof(unsigned long long), SEEK_SET);

		//int c;
		//while ((c = fgetc(tmp)) != EOF)
		//{
		//	fputc(c, out);
		//}

		while (!feof(tmp))
		{
			size_t bread_count = fread(tbuf, sizeof(char), TBUFSIZE, tmp);
			fwrite(tbuf, sizeof(char), bread_count, out);
		}

		int error = ferror(tmp);
		if (error != 0)
		{
			fprintf(stderr, "Error: unexpected eof while reading file %s; ferror = %d\n", filename_arr[i], ferror(tmp));
		}

		fclose(tmp);
		remove(filename_arr[i]);
	}

	fflush(out);
	fclose(out);

	return 0;
}
