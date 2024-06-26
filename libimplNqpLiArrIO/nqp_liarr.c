#include "pch.h"

#include "nqp_io_liarr.h"
#include "nqp_liarr.h"

#include "nqp_fail_alloc_check.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void _nqp_liarr_element_write_solution(nqp_liarr_element * element,
	int dim, int * solution);

nqp_liarr_element * nqp_liarr_element_new(int dim, unsigned long long solution_capacity)
{
	nqp_liarr_element * element = (nqp_liarr_element *)malloc(sizeof(nqp_liarr_element));
	nqp_fail_alloc_check(element);

	element->solution_capacity = solution_capacity;
	element->solution_fill = 0;
	element->solutions = (int *)malloc(dim * solution_capacity * sizeof(int));
	element->solution_write_ptr = element->solutions;
	element->next = NULL;

	return element;
}

void nqp_liarr_element_free(nqp_liarr_element * element)
{
	free(element->solutions);
	free(element);
}

nqp_liarr * nqp_liarr_new(int solution_capacity_incr_coef,
	int head_dim, unsigned long long head_solution_capacity)
{
	nqp_liarr * liarr = (nqp_liarr *)malloc(sizeof(nqp_liarr));
	nqp_fail_alloc_check(liarr);

	liarr->solution_capacity_incr_coef = solution_capacity_incr_coef;

	liarr->head = nqp_liarr_element_new(head_dim, head_solution_capacity);
	nqp_fail_alloc_check(liarr->head);
	liarr->last = liarr->head;

	return liarr;
}

void nqp_liarr_free(nqp_liarr * liarr)
{
	free(liarr);
}

void nqp_liarr_append(nqp_liarr * liarr, int dim, int * solution)
{
	if (liarr->last->solution_fill < liarr->last->solution_capacity)
	{
		_nqp_liarr_element_write_solution(liarr->last, dim, solution);
	}
	else
	{
		nqp_liarr_element * next =
			nqp_liarr_element_new(dim, liarr->solution_capacity_incr_coef * liarr->last->solution_capacity);
		_nqp_liarr_element_write_solution(next, dim, solution);
		liarr->last->next = next;
		liarr->last = next;
	}
}

nqp_liarr_element * nqp_liarr_pop(nqp_liarr * liarr)
{
	if (liarr->head == NULL)
	{
		return NULL;
	}
	else
	{
		nqp_liarr_element * result = liarr->head;
		liarr->head = liarr->head->next;
		return result;
	}
}

static void _nqp_liarr_element_write_solution(nqp_liarr_element * element,
	int dim, int * solution)
{
	errno_t error = memcpy_s(element->solution_write_ptr, dim * sizeof(int),
		solution, dim * sizeof(int));
	if (error != 0)
	{
		fprintf(stderr, "Error: failed to copy solution into liarr, errno = %d\n", error);
	}
	element->solution_fill++;
	element->solution_write_ptr += dim;
}