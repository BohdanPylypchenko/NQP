#include "pch.h"

#include "nqp_field.h"

#include "nqp_fail_alloc_check.h"
#include "WinapiConfig.h"

#include <stdio.h>
#include <stdlib.h>

int ** field_alloc(
	int dim,
	HANDLE heap
) {
	int ** field = (int **)HeapAlloc(heap, 0, dim * sizeof(int *));
	nqp_fail_alloc_check(field);

	for (int i = 0; i < dim; i++)
	{
		field[i] = (int *)HeapAlloc(heap, 0, dim * sizeof(int));
		nqp_fail_alloc_check(field[i]);
		for (int j = 0; j < dim; field[i][j++] = 0);
	}

	return field;
}
