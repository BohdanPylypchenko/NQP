#include "pch.h"

#include "write_concat.h"
#include "file_buffer_adjust.h"
#include "file_buffer_adjust_const.h"
#include "nqp_io_const.h"
#include "nqp_null_check.h"
#include "WinapiConfig.h"

#include <stdio.h>

#define TBUFSIZE 4096

int write_concat(
	int dim,
	unsigned long long total_solution_count
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
		fprintf(stderr, "Error: can't resize terminal file memory_map size\n");
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

	WIN32_FIND_DATA ffd;
	HANDLE find = FindFirstFile(L".\\*.tnqp", &ffd);
	if (find == INVALID_HANDLE_VALUE)
	{
		abort();
	}

	do
	{
		HANDLE file = CreateFileW(
			ffd.cFileName,
			GENERIC_READ,
			0, NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
		if (file == INVALID_HANDLE_VALUE)
		{
			abort();
		}

		HANDLE fileMapping = CreateFileMappingW(
			file,
			NULL,
			PAGE_READONLY,
			0, 0,
			NULL
		);
		if (fileMapping == NULL)
		{
			abort();
		}

		LPBYTE fileView = MapViewOfFile(
			fileMapping,
			FILE_MAP_READ,
			0, 0, 0
		);

		CloseHandle(fileMapping);

		LPBYTE data = fileView + sizeof(int);
		unsigned long long solution_count;
		memcpy(&solution_count, data, sizeof(unsigned long long));
		data += sizeof(unsigned long long);

		size_t solution_size = dim * sizeof(int);
		for (unsigned long long i = 0; i < solution_count; i++)
		{
			fwrite(data, sizeof(int), dim, out);
			data += solution_size;
		}

		UnmapViewOfFile(fileView);

		CloseHandle(file);
		DeleteFileW(ffd.cFileName);
	} while (FindNextFile(find, &ffd) != 0);

	fflush(out);
	fclose(out);

	return 0;
}
