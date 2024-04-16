#pragma once

#include <stdio.h>

int adjust_buf_size(
	FILE * stream, int dim,
	int bufsize_11, int bufsize_15, int bufsize_big
);
