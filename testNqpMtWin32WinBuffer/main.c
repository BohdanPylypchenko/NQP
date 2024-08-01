#include "pch.h"

#include "nqp_test.h"
#include "nqp_mt_win32_winbuffer.h"

#define MAX_DIM 16
#define THREAD_COUNT 8

unsigned long long nqp_solver(int dim, int thread_count)
{
	return nqp_mt_win32_winbuffer(dim, thread_count);
}

int main(void)
{
	nqp_test(MAX_DIM, THREAD_COUNT);
}