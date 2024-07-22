#include "pch.h"

#include "file_buffer_adjust.h"
#include <stdio.h>

int adjust_buf_size(
	FILE * stream, int dim,
	int bufsize_11, int bufsize_15, int bufsize_big
) {
	if (dim < 8)
		return 0;

	size_t bufsize;
	if (dim <= 11)
		bufsize = bufsize_11;
	else if (dim <= 15)
		bufsize = bufsize_15;
	else
		bufsize = bufsize_big;

	return setvbuf(stream, NULL, _IOFBF, bufsize);
}
