#pragma once

#include "nqp_io.h"
#include "WinapiConfig.h"

typedef struct nqp_queue_element
{
	int dim;
	int * solution;
	struct nqp_queue_element * next;
} nqp_queue_element;

typedef struct nqp_writer
{
	nqp_queue_element * head;
	nqp_queue_element * last;
	HANDLE heap;
} nqp_queue;

struct nqp_start_args
{
	int dim;
	int queue_count;
	char * out_filename;
	nqp_queue ** queue_arr;
	int bufsize_11;
	int bufsize_15;
	int bufsize_big;
};
