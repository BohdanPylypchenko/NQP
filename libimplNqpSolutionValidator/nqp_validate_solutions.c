#include "pch.h"

#include "nqp_validate_solutions.h"
#include "nqp_iteration.h"
#include "nqp_field.h"
#include "nqp_io.h"
#include "nqp_fail_alloc_check.h"

#include <stdio.h>
#include <stdlib.h>

int nqp_validate_solutions(
	int expected_dim, unsigned long long expected_solution_count,
	FILE * solution_stream
) {
	int actual_dim = -1;
	fread(&actual_dim, sizeof(int), 1, solution_stream);
	if (expected_dim != actual_dim)
	{
		printf("Solution validation error: expected_dim = %d, actual_dim = %d\n", expected_dim, actual_dim);
		return 1;
	}

	HANDLE heap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
	nqp_fail_alloc_check(heap);

	size_t read_count;
	unsigned long long actual_solution_count = 0;
	int * solution = (int *)HeapAlloc(heap, 0, actual_dim * sizeof(int));
	nqp_fail_alloc_check(solution);
	int ** field = field_alloc(actual_dim, heap);
	unsigned long long invalid_solution_count = 0;
	while ((read_count = fread(solution, sizeof(int), actual_dim, solution_stream)) == actual_dim)
	{
		actual_solution_count++;

		for (int i = 0; i < actual_dim; i++)
			mark_fields(actual_dim, field, i, solution[i], 1);

		for (int i = 0; i < actual_dim; i++)
			if (field[i][solution[i]] != 1)
				invalid_solution_count++;

		for (int i = 0; i < actual_dim; i++)
			for (int j = 0; j < actual_dim; j++)
				field[i][j] = 0;
	}
	HeapDestroy(heap);

	if (ferror(solution_stream))
	{
		printf("Error while reading solutions, actual solution count = %llu, invalid solution count = %llu\n",
			actual_solution_count, invalid_solution_count);
		return 1;
	}
	else if (feof(solution_stream))
	{
		printf("End of stream has been reached, actual solution count = %llu, invalid solution count = %llu\n",
			actual_solution_count, invalid_solution_count);
		return 0;
	}
	else
	{
		printf("Undefined solution stream state, actual solution count = %llu, invalid solution count = %llu\n",
			actual_solution_count, invalid_solution_count);
		return 1;
	}
}

void nqp_write_solution(int dim, int * solution, nqp_writer * writer) {}