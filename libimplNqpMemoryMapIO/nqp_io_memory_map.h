#pragma once

#include "nqp_io.h"

#include "WinapiConfig.h"
#include "nqp_memory_map.h"

#include <stdio.h>

struct nqp_writer
{
	unsigned long long id;
	HANDLE heap;
	HANDLE file;
	int dim;
	unsigned long long solution_count;
	unsigned long long current_memory_map_id;
	unsigned long long memory_size_in_bytes;
	nqp_memory_map * memory_map;
};

struct nqp_init_args
{
	unsigned long long id;
	unsigned long long memory_size_in_bytes;
	int dim;
};
