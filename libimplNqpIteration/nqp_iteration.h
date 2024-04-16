#pragma once

#include "nqp_io.h"

typedef struct
{
    int dim;

    int ** field;
    int * queens;

    unsigned long long s_count;
    nqp_writer * writer;
} nqp_state;

void nqp_iteration(int q_index, nqp_state * state);
void mark_fields(int dim, int ** field, int h_index, int v_index, int action_value);

