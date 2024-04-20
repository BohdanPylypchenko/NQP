#include "pch.h"

#include "nqp_mt.h"
#include "nqp_io.h"
#include "nqp_iteration.h"
#include "nqp_field.h"
#include "nqp_fail_alloc_check.h"

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

typedef struct {
    int thread_index;
    DWORD thread_id;
    nqp_state * state;
} task;

DWORD WINAPI _thread_execute_task(LPVOID lpParam) {
    task * t = (task *)lpParam;

    printf(
        "Thread index = %d; Thread id = %d;\n",
        t->thread_index, t->thread_id
    );

    double start_time = omp_get_wtime();
    nqp_iteration(1, t->state);
    double end_time = omp_get_wtime();
    printf(
        "Thread %d finish in %lf\n",
        t->thread_index, end_time - start_time
    );

    return 0;
}

unsigned long long nqp_mt(
    int dim,
    int thread_count,
    nqp_writer ** writer_arr,
    nqp_start_args * start_args
) {
    unsigned long long total_s_count = 0;
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

    task ** task_ptr_arr = (task **)malloc(dim * sizeof(task *));
    nqp_fail_alloc_check(task_ptr_arr);
    for (int i = 0; i < dim; i++) {
        task_ptr_arr[i] = (task *)malloc(sizeof(task));
        nqp_fail_alloc_check(task_ptr_arr[i]);
        task_ptr_arr[i]->thread_index = i;
        task_ptr_arr[i]->state = state_ptr_arr[i];
    }

    PHANDLE hThreadArr = (PHANDLE)malloc(dim * sizeof(HANDLE));
    nqp_fail_alloc_check(hThreadArr);
    nqp_write_start(start_args);
    double start_time = omp_get_wtime();
    for (int i = 0; i < dim; i++) {
        hThreadArr[i] = CreateThread(
            NULL,                         // default security attributes
            0,                            // use default stack size  
            _thread_execute_task,         // thread function name
            task_ptr_arr[i],              // argument to thread function 
            0,                            // use default creation flags 
            &(task_ptr_arr[i]->thread_id) // returns the thread identifier
        );
        if (hThreadArr[i] == NULL) {
            fprintf(stderr, "Error: failed to create thread, code = %d\n", GetLastError());
            ExitProcess(3);
        }
    }
    WaitForMultipleObjects(dim, hThreadArr, TRUE, INFINITE);
    for (int i = 0; i < dim; i++) {
        total_s_count += task_ptr_arr[i]->state->s_count;
    }
    double end_time = omp_get_wtime();
    printf("Comput time: %lf\n", end_time - start_time);

    nqp_write_notify_computation_complete(total_s_count);
    nqp_write_wait();
    nqp_write_end();

    for (int i = 0; i < dim; i++) {
        CloseHandle(hThreadArr[i]);
    }
    free(hThreadArr);

    for (int i = 0; i < dim; i++) {
        field_free(state_ptr_arr[i]->field, dim);
        free(state_ptr_arr[i]->queens);
        free(state_ptr_arr[i]);
    }
    free(state_ptr_arr);

    for (int i = 0; i < dim; i++) {
        free(task_ptr_arr[i]);
    }
    free(task_ptr_arr);

    return total_s_count;
}
