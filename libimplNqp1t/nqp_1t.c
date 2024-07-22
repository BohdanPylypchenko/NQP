#include "pch.h"

#include "nqp_1t.h"

#include "nqp_io.h"
#include "nqp_null_check.h"
#include "nqp_iteration.h"
#include "nqp_field.h"

#include <stdio.h>

unsigned long long nqp_1t(
    int dim,
    nqp_writer * writer,
    nqp_start_args * start_args
) {
    HANDLE heap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
    nqp_null_check(heap);

    nqp_state state =
    {
        .dim = dim,
        .field = field_alloc(dim, heap),
        .queens = (int *)HeapAlloc(heap, 0, dim * sizeof(int)),
        .s_count = 0,
        .writer = writer
    };
    nqp_null_check(state.queens);

    if (nqp_write_start(start_args) != 0)
    {
        fprintf(stderr, "Error while starting write operation\n");
        return -1;
    }
    nqp_iteration(0, &state);
    nqp_write_notify_computation_complete(state.s_count);
    nqp_write_wait();

    HeapDestroy(heap);

    return state.s_count;
}