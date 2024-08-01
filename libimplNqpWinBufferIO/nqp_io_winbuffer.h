#pragma once

#include "nqp_io.h"
#include "WinapiConfig.h"

typedef struct nqp_writer
{
	unsigned long long id;
	HANDLE file;
	HANDLE heap;
	size_t size_in_bytes;
	size_t count_of_used_bytes;
	char * buffer;
	char * write_ptr;
	unsigned long long solution_count;
} nqp_winbuffer;

struct nqp_init_args
{
	unsigned long long id;
	size_t size_in_bytes;
	int dim;
};

struct nqp_start_args
{
	char reserved[8];
};
