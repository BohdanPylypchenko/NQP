#include "pch.h"

#include "nqp_field.h"

#include "nqp_fail_alloc_check.h"

#include <stdio.h>
#include <stdlib.h>

int ** field_alloc(int dim)
{
	int ** field = (int **)malloc(dim * sizeof(int *));
	nqp_fail_alloc_check(field);

	for (int i = 0; i < dim; i++)
	{
		field[i] = (int *)calloc(dim, sizeof(int));
		nqp_fail_alloc_check(field[i]);
	}

	return field;
}

void field_free(int ** field, int dim)
{
	for (int i = 0; i < dim; i++)
	{
		free(field[i]);
	}
	free(field);
}