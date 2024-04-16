#include "pch.h"

#include "nqp_test.h"
#include "nqp_1t_liarr.h"

#define MAX_DIM 16

unsigned long long nqp_solver(int dim, int thread_count)
{
	return nqp_1t_liarr(dim);
}

int main(void)
{
	nqp_test(MAX_DIM, 0);
	return 0;
}