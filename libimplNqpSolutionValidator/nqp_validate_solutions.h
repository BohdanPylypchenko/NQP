#pragma once

#include <stdio.h>

int nqp_validate_solutions(
	int expected_dim,
	unsigned long long expected_solution_count,
	FILE * solution_stream
);
