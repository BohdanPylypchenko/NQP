#include "pch.h"

#include "nqp_mt.h"
#include "nqp_io.h"
#include "nqp_iteration.h"
#include "nqp_field.h"
#include "nqp_fail_alloc_check.h"

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

static const DWORD MIN_THREAD_COUNT = 4;
static const DWORD THREAD_COUNT_MULTIPLIER = 4;

VOID
CALLBACK
MyWorkCallback(
    PTP_CALLBACK_INSTANCE Instance,
    PVOID                 Parameter,
    PTP_WORK              Work
) {
    UNREFERENCED_PARAMETER(Instance);
    UNREFERENCED_PARAMETER(Work);

    double start_time = omp_get_wtime();
    nqp_state * state = (nqp_state *)Parameter;
    nqp_iteration(1, state);
    double end_time = omp_get_wtime();
    printf("work finished in %lf\n", end_time - start_time);
}

unsigned long long nqp_mt(
    int dim,
    int thread_count,
    nqp_writer ** writer_arr,
    nqp_start_args * start_args
) {
    BOOL bRet = FALSE;
    PTP_WORK work = NULL;
    PTP_TIMER timer = NULL;
    PTP_POOL pool = NULL;
    PTP_WORK_CALLBACK workcallback = MyWorkCallback;
    TP_CALLBACK_ENVIRON CallBackEnviron;
    PTP_CLEANUP_GROUP cleanupgroup = NULL;

    InitializeThreadpoolEnvironment(&CallBackEnviron);

    pool = CreateThreadpool(NULL);
    if (NULL == pool) {
        printf(
            "CreateThreadpool failed. LastError: %u\n",
            GetLastError()
        );
        abort();
    }

    SetThreadpoolThreadMaximum(pool, thread_count * THREAD_COUNT_MULTIPLIER);
    bRet = SetThreadpoolThreadMinimum(pool, MIN_THREAD_COUNT);
    if (FALSE == bRet) {
        printf("SetThreadpoolThreadMinimum failed. LastError: %u\n",
            GetLastError());
        abort();
    }

    cleanupgroup = CreateThreadpoolCleanupGroup();
    if (NULL == cleanupgroup) {
        printf("CreateThreadpoolCleanupGroup failed. LastError: %u\n",
            GetLastError());
        abort();
    }

    SetThreadpoolCallbackPool(&CallBackEnviron, pool);
    SetThreadpoolCallbackCleanupGroup(
        &CallBackEnviron,
        cleanupgroup,
        NULL
    );

    nqp_state ** state_ptr_arr = (nqp_state **)malloc(dim * sizeof(nqp_state *));
    nqp_fail_alloc_check(state_ptr_arr);
    for (int i = 0; i < dim; i++) {
        state_ptr_arr[i] = (nqp_state *)malloc(sizeof(nqp_state));
        nqp_fail_alloc_check(state_ptr_arr[i]);
        state_ptr_arr[i]->dim = dim;
        state_ptr_arr[i]->field = field_alloc(dim);
        state_ptr_arr[i]->queens = (int *)malloc(dim * sizeof(int));
        nqp_fail_alloc_check(state_ptr_arr[i]->queens);
        state_ptr_arr[i]->s_count = 0;
        state_ptr_arr[i]->writer = writer_arr[i];

        state_ptr_arr[i]->queens[0] = i;
        mark_fields(dim, state_ptr_arr[i]->field, 0, i, 1);
    }

    nqp_write_start(start_args);
    double start_time_total = omp_get_wtime();
    for (int i = 0; i < dim; i++) {
        work = CreateThreadpoolWork(
            workcallback,
            state_ptr_arr[i],
            &CallBackEnviron
        );
        if (NULL == work) {
            printf(
                "CreateThreadpoolWork failed. LastError: %u; i = %d\n",
                GetLastError(),
                i
            );
            abort();
        }

        SubmitThreadpoolWork(work);
    }
    CloseThreadpoolCleanupGroupMembers(
        cleanupgroup,
        FALSE,
        NULL
    );
    CloseThreadpoolCleanupGroup(cleanupgroup);
    CloseThreadpool(pool);
    unsigned long long total_s_count = 0;
    for (int i = 0; i < dim; i++) {
        total_s_count += state_ptr_arr[i]->s_count;
    }
    double end_time_total = omp_get_wtime();
    printf("Comput time: %lf\n", end_time_total - start_time_total);

    nqp_write_notify_computation_complete(total_s_count);
    nqp_write_wait();
    nqp_write_end();

    for (int i = 0; i < dim; i++) {
        field_free(state_ptr_arr[i]->field, dim);
        free(state_ptr_arr[i]->queens);
        free(state_ptr_arr[i]);
    }
    free(state_ptr_arr);

    return total_s_count;
}