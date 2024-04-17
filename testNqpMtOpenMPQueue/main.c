#include "pch.h"

#include "nqp_test.h"
#include "nqp_mt_omp_queue.h"

#define MAX_DIM 16
#define THREAD_COUNT 6

unsigned long long nqp_solver(int dim, int thread_count)
{
	return nqp_mt_omp_queue(dim, thread_count);
}

int main(void)
{
	nqp_test(MAX_DIM, THREAD_COUNT);
}