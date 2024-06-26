#include "pch.h"

#include "nqp_io_queue.h"
#include "nqp_queue.h"

#include "nqp_fail_alloc_check.h"

#include <stdlib.h>

nqp_queue_element * nqp_queue_element_new(int dim, int * solution)
{
	nqp_queue_element * element = (nqp_queue_element *)malloc(sizeof(nqp_queue_element));
	nqp_fail_alloc_check(element);

	element->dim = dim;
	element->solution = solution;
	element->next = NULL;

	return element;
}

void nqp_queue_element_free(nqp_queue_element * element)
{
	free(element->solution);
	free(element);
}

static nqp_queue_element * nqp_queue_get_headnext(nqp_queue * queue);

nqp_queue * nqp_queue_new()
{
	nqp_queue * queue = (nqp_queue *)malloc(sizeof(nqp_queue));
	nqp_fail_alloc_check(queue);

	queue->head = (nqp_queue_element *)malloc(sizeof(nqp_queue_element));
	nqp_fail_alloc_check(queue->head);
	queue->head->dim = -1;
	queue->head->solution = NULL;
	queue->head->next = NULL;
	queue->last = queue->head;

	return queue;
}

void nqp_queue_free(nqp_queue * queue)
{
	free(queue->head);
	free(queue);
}

void nqp_queue_append(nqp_queue * queue, nqp_queue_element * element)
{
	queue->last->next = element;
	queue->last = element;
}

nqp_queue_element * nqp_queue_pop(nqp_queue * queue)
{
	if (queue->head->next == NULL)
		return NULL;
	else
		return nqp_queue_get_headnext(queue);
}

nqp_queue_element * nqp_queue_pop_safe(nqp_queue * queue)
{
	if ((queue->head->next == NULL) || (queue->head->next == queue->last))
		return NULL;
	else
		return nqp_queue_get_headnext(queue);
}

static nqp_queue_element * nqp_queue_get_headnext(nqp_queue * queue)
{
	nqp_queue_element * result = queue->head->next;
	queue->head->next = result->next;
	return result;
}
