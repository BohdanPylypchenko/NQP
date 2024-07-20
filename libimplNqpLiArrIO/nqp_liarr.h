#pragma once

#include "nqp_io_liarr.h"
#include "WinapiConfig.h"

nqp_liarr_element * nqp_liarr_element_new(
	int dim, unsigned long long solution_capacity,
	HANDLE heap
);

void nqp_liarr_element_free(
	nqp_liarr_element * element,
	HANDLE heap
);

nqp_liarr * nqp_liarr_new(
	int solution_capacity_incr_coef,
	int head_dim, unsigned long long head_solution_capacity
);
void nqp_liarr_free(nqp_liarr * liarr);

void nqp_liarr_append(nqp_liarr * liarr, int dim, int * solution);

nqp_liarr_element * nqp_liarr_pop(nqp_liarr * liarr);
