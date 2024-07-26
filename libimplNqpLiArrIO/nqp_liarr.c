#include "pch.h"

#include "nqp_io_liarr.h"
#include "nqp_liarr.h"

#include "nqp_null_check.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void _nqp_liarr_element_write_solution(nqp_liarr_element * element,
	int dim, int * solution);

nqp_liarr_element * nqp_liarr_element_new(
	int dim, unsigned long long solution_capacity,
	HANDLE heap
) {
	nqp_liarr_element * element = (nqp_liarr_element *)HeapAlloc(heap, 0, sizeof(nqp_liarr_element));
	nqp_null_check(element);

	element->solution_capacity = solution_capacity;
	element->solution_fill = 0;

	element->solutions = (int *)HeapAlloc(heap, 0, dim * solution_capacity * sizeof(int));
	nqp_null_check(element->solutions);

	element->solution_write_ptr = element->solutions;
	element->next = NULL;

	return element;
}

void nqp_liarr_element_free(
	nqp_liarr_element * element,
	HANDLE heap
) {
	HeapFree(heap, 0, element->solutions);
	HeapFree(heap, 0, element);
}

nqp_liarr * nqp_liarr_new(
	int solution_capacity_incr_coef,
	int head_dim, unsigned long long head_solution_capacity
) {
	HANDLE heap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
	nqp_null_check(heap);

	nqp_liarr * liarr = (nqp_liarr *)HeapAlloc(heap, 0, sizeof(nqp_liarr));
	nqp_null_check(liarr);
	liarr->heap = heap;

	liarr->solution_capacity_incr_coef = solution_capacity_incr_coef;

	liarr->head = nqp_liarr_element_new(head_dim, head_solution_capacity, heap);
	nqp_null_check(liarr->head);
	liarr->last = liarr->head;

	return liarr;
}

void nqp_liarr_free(nqp_liarr * liarr)
{
	HANDLE heap = liarr->heap;
	HeapDestroy(heap);
}

void nqp_liarr_append(nqp_liarr * liarr, int dim, int * solution)
{
	if (liarr->last->solution_fill < liarr->last->solution_capacity)
	{
		_nqp_liarr_element_write_solution(liarr->last, dim, solution);
	}
	else
	{
		nqp_liarr_element * next = nqp_liarr_element_new(
			dim, liarr->solution_capacity_incr_coef * liarr->last->solution_capacity,
			liarr->heap
		);
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

static void _nqp_liarr_element_write_solution(
	nqp_liarr_element * element,
	int dim, int * solution
) {
	memcpy(element->solution_write_ptr, solution, dim * sizeof(int));

	element->solution_fill++;
	element->solution_write_ptr += dim;
}