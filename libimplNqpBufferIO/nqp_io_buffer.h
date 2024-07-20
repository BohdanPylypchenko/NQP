#pragma once

#include "nqp_io.h"

#include "WinapiConfig.h"

#include <stdio.h>

struct nqp_writer
{
	unsigned long long id;
	char * out_filename;
	FILE * out;
	HANDLE heap;
};

struct nqp_init_args
{
	unsigned long long id;
	char * out_filename;
	int dim;
	int bufsize_11;
	int bufsize_15;
	int bufsize_big;
};
