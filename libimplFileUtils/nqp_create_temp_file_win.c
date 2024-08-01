#include "pch.h"

#include "nqp_create_temp_file_win.h"

#include "WinapiConfig.h"
#include "nqp_io_const.h"

#include <stdio.h>

HANDLE create_temp_file_win(
	unsigned long long primary_id,
	unsigned long long secondary_id
) {
	WCHAR temp_file_name[NQP_TEMP_FILE_NAME_LENGTH];
	swprintf(
		temp_file_name, NQP_TEMP_FILE_NAME_LENGTH, NQP_TEMP_FILE_NAME_FORMAT_W,
	    GetCurrentProcessId(), GetCurrentThreadId(), primary_id, secondary_id,
		NQP_TEMP_EXTENSION_W
	);

	HANDLE file = CreateFileW(
		temp_file_name,
		GENERIC_READ | GENERIC_WRITE,
		0, NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (file == INVALID_HANDLE_VALUE)
	{
		fwprintf(
			stderr, L"Error: can't create temp file %s; Error code = %d\n",
			temp_file_name, GetLastError()
		);
	}
	
	return file;
}