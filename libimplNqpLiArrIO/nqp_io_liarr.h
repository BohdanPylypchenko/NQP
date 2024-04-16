#pragma once

#include "nqp_io.h"

typedef struct nqp_liarr_element
{
	unsigned long long solution_capacity;
	unsigned long long solution_fill;
	int * solution_write_ptr;
	int * solutions;
	struct nqp_liarr_element * next;
} nqp_liarr_element;

typedef struct nqp_writer
{
	int solution_capacity_incr_coef;
	nqp_liarr_element * head;
	nqp_liarr_element * last;
} nqp_liarr;

struct nqp_init_args
{
	int dim;
	int solution_capacity_incr_coef;
	unsigned long long head_solution_capacity;
};

struct nqp_start_args
{
	int dim;
	int liarr_count;
	nqp_liarr ** liarr_arr;
	char * out_filename;
	int bufsize_11;
	int bufsize_15;
	int bufsize_big;
};
