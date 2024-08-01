#pragma once

#include <stdio.h>

#define INJECT_DIM_SUCCESS 2
#define INJECT_TOTAL_SOLUTION_COUNT_SUCCESS 1

int inject_dim(int dim, FILE * f);

int inject_total_solution_count(unsigned long long s_count, FILE * f);
