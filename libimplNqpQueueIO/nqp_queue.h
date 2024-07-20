#pragma once

#include "nqp_io_queue.h"
#include "WinapiConfig.h"

nqp_queue_element * nqp_queue_element_new(
    int dim, int * solution,
    HANDLE heap
);

void nqp_queue_element_free(
    nqp_queue_element * element,
    HANDLE heap
);

nqp_queue * nqp_queue_new();
void nqp_queue_free(nqp_queue * queue);

void nqp_queue_append(nqp_queue * queue, nqp_queue_element * element);
nqp_queue_element * nqp_queue_pop(nqp_queue * queue);
nqp_queue_element * nqp_queue_pop_safe(nqp_queue * queue);
