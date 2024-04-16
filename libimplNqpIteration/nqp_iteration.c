#include "pch.h"

#include "nqp_iteration.h"

#include "nqp_io.h"
#include "nqp_fail_alloc_check.h"

#include <stdlib.h>
#include <string.h>

void nqp_iteration(int q_index, nqp_state * state)
{
    if (q_index == state->dim)
    {
        state->s_count++;

        nqp_write_solution(state->dim, state->queens, state->writer);

        return;
    }

    for (int v = 0; v < state->dim; v++)
    {
        if (state->field[q_index][v] != 0)
            continue;

        state->queens[q_index] = v;
        mark_fields(state->dim, state->field, q_index, v, 1);

        nqp_iteration(q_index + 1, state);

        mark_fields(state->dim, state->field, q_index, v, -1);
    }
}

void mark_fields(int dim, int ** field, int h_index, int v_index, int action_value)
{
    for (int i = 0; i < dim; i++)
    {
        field[i][v_index] += action_value;
        field[h_index][i] += action_value;
    }
    field[h_index][v_index] -= action_value;

    for (int h = h_index + 1, v = v_index + 1; h < dim && v < dim; h++, v++)
    {
        field[h][v] += action_value;
    }
    for (int h = h_index - 1, v = v_index - 1; h >= 0 && v >= 0; h--, v--)
    {
        field[h][v] += action_value;
    }

    for (int h = h_index - 1, v = v_index + 1; h >= 0 && v < dim; h--, v++)
    {
        field[h][v] += action_value;
    }
    for (int h = h_index + 1, v = v_index - 1; h < dim && v >= 0; h++, v--)
    {
        field[h][v] += action_value;
    }
}