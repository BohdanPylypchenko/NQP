#include "pch.h"

#include "nqp_create_temp_file_std.h"

#include "nqp_io_const.h"
#include "WinapiConfig.h"

#include <stdio.h>

FILE * create_temp_file_std(
	unsigned long long primary_id,
	unsigned long long secondary_id
) {
	char temp_file_name[NQP_TEMP_FILE_NAME_LENGTH];
	sprintf_s(
		temp_file_name, NQP_TEMP_FILE_NAME_LENGTH, NQP_TEMP_FILE_NAME_FORMAT,
		GetCurrentProcessId(), GetCurrentThreadId(), primary_id, secondary_id,
		NQP_TEMP_EXTENSION
	);

	FILE * f;
	errno_t error = fopen_s(&f, temp_file_name, "wb");
	if (error != 0)
	{
		fprintf(
			stderr, "Error: can't create temp file %s; Error code = %d\n",
			temp_file_name, error
		);
		return NULL;
	}
	else
	{
		return f;
	}
}