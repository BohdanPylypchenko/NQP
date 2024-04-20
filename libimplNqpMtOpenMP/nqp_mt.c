#include "pch.h"

#include "nqp_io.h"
#include "nqp_mt.h"
#include "nqp_iteration.h"
#include "nqp_field.h"
#include "nqp_fail_alloc_check.h"

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

unsigned long long nqp_mt(
    int dim,
    int thread_count,
    nqp_writer ** writer_arr,
    nqp_start_args * start_args
) {
    unsigned long long total_s_count = 0;
    nqp_state * state_arr = (nqp_state *)malloc(dim * sizeof(nqp_state));
    nqp_fail_alloc_check(state_arr);
    nqp_write_start(start_args);
    int i;
    omp_set_num_threads(thread_count);
    double start_time_total = omp_get_wtime();
    #pragma omp parallel for reduction(+:total_s_count)
    for (i = 0; i < dim; i++) {
        state_arr[i].dim = dim;
        state_arr[i].field = field_alloc(dim);
        state_arr[i].queens = (int *)malloc(dim * sizeof(int));
        nqp_fail_alloc_check(state_arr[i].queens);
        state_arr[i].s_count = 0;
        state_arr[i].writer = writer_arr[i];

        state_arr[i].queens[0] = i;
        mark_fields(dim, state_arr[i].field, 0, i, 1);

        double start_time = omp_get_wtime();
        nqp_iteration(1, &(state_arr[i]));
        total_s_count += state_arr[i].s_count;
        double end_time = omp_get_wtime();
        printf(
            "Iteration %d finished in %lf\n",
            i, end_time - start_time
        );
    }
    double end_time_total = omp_get_wtime();
    printf("Comput time: %lf\n", end_time_total - start_time_total);

    nqp_write_notify_computation_complete(total_s_count);
    nqp_write_wait();
    nqp_write_end();

    for (int i = 0; i < dim; i++) {
        field_free(state_arr[i].field, dim);
        free(state_arr[i].queens);
    }
    free(state_arr);

    return total_s_count;
}