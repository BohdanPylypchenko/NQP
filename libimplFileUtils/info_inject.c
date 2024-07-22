#include "pch.h"

#include "nqp_inject.h"

#include <stdio.h>

int inject_dim(int dim, FILE * f)
{
	int dim_result = (int)fwrite(&dim, sizeof(int), 1, f);

	unsigned long long zero = 0;
	int zero_result = (int)fwrite(&zero, sizeof(unsigned long long), 1, f);

	return dim_result + zero_result;
}

int inject_total_solution_count(unsigned long long s_count, FILE * f)
{
	int fseek_result = (int)fseek(f, sizeof(int), SEEK_SET);
	return (int)fwrite(&s_count, sizeof(unsigned long long), 1, f) + fseek_result;
}