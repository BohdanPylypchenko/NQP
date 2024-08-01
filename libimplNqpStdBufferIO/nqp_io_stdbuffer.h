#pragma once

#include "nqp_io.h"

#include "WinapiConfig.h"

#include <stdio.h>

struct nqp_writer
{
	HANDLE heap;
	FILE * out;
	unsigned long long id;
	unsigned long long solution_count;
};

struct nqp_init_args
{
	unsigned long long id;
	int dim;
	int bufsize_11;
	int bufsize_15;
	int bufsize_big;
};
