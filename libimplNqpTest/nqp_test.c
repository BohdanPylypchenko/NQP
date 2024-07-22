#include "pch.h"

#include "nqp_test.h"

#include "nqp_validate_solutions.h"

#include <stdio.h>
#include <omp.h>

#define MIN_N 8
#define SHIFT 4

#define OUT_NAME "out.nqp"

static const unsigned long long EXPECTED_SOLUTION_COUNT[] =
{
	2, 10, 4, 40, 92, 352, 724, 2680, 14200, 73712, 365596, 2279184, 14772512, 95815104, 666090624
};

int nqp_test(int max_dim, int thread_count)
{
	int is_failed = 0;
	unsigned long long actual_count;
	double time;
	printf("<<< TEST <<<\n");
	for (int i = MIN_N; i <= max_dim; i++)
	{
		printf("\n=============== n = %d ===============\n", i);

		double begin = omp_get_wtime();
		actual_count = nqp_solver(i, thread_count);
		double end = omp_get_wtime();

		time = (double)(end - begin);

		if (EXPECTED_SOLUTION_COUNT[i - SHIFT] == actual_count)
		{
			printf("Solution count test passed\n");
		}
		else
		{
			printf("Solution count test failed\n");
			is_failed++;
		}
		printf("Time = %lf\n", time);

		FILE * result;
		fopen_s(&result, OUT_NAME, "rb");
		int validation_flag = nqp_validate_solutions(i, actual_count, result);
		if (validation_flag == 0)
			printf("Solution validation passed for dim = %d\n", i);
		else
			printf("Solution validation failed for dim = %d\n", i);
		fclose(result);

		remove(OUT_NAME);
	}

	return is_failed;
}