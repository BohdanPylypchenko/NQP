#include "pch.h"

#include "nqp_mt.h"
#include "nqp_io.h"
#include "nqp_iteration.h"
#include "nqp_field.h"
#include "nqp_null_check.h"
#include "WinapiConfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

typedef struct {
    int thread_index;
    DWORD thread_id;
    nqp_state * state;
    HANDLE task_heap;
} task;

static DWORD WINAPI _thread_execute_task(LPVOID lpParam);

unsigned long long nqp_mt(
    int dim,
    int thread_count,
    nqp_writer ** writer_arr,
    nqp_start_args * start_args
) {
    HANDLE global_heap = HeapCreate(0, 0, 0);
    nqp_null_check(global_heap);

    task ** task_ptr_arr = (task **)HeapAlloc(global_heap, 0, dim * sizeof(task *));
    nqp_null_check(task_ptr_arr);
    for (int i = 0; i < dim; i++)
    {
        HANDLE task_heap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
        nqp_null_check(task_heap);

        task_ptr_arr[i] = (task *)HeapAlloc(task_heap, 0, sizeof(task));
        nqp_null_check(task_ptr_arr[i]);

        task_ptr_arr[i]->task_heap = task_heap;
        task_ptr_arr[i]->thread_index = i;

        task_ptr_arr[i]->state = (nqp_state *)HeapAlloc(task_heap, 0, sizeof(nqp_state));
        nqp_null_check(task_ptr_arr[i]->state);

        task_ptr_arr[i]->state->dim = dim;
        task_ptr_arr[i]->state->field = field_alloc(dim, task_heap);

        task_ptr_arr[i]->state->queens = (int *)HeapAlloc(task_heap, 0, dim * sizeof(int));
        nqp_null_check(task_ptr_arr[i]->state->queens);

        task_ptr_arr[i]->state->s_count = 0;
        task_ptr_arr[i]->state->writer = writer_arr[i];

        task_ptr_arr[i]->state->queens[0] = i;
        mark_fields(dim, task_ptr_arr[i]->state->field, 0, i, 1);
    }

    PHANDLE hThreadArr = (PHANDLE)HeapAlloc(global_heap, 0, dim * sizeof(HANDLE));
    nqp_null_check(hThreadArr);
    nqp_write_start(start_args);
    double start_time = omp_get_wtime();
    for (int i = 0; i < dim; i++)
    {
        hThreadArr[i] = CreateThread(
            NULL,                         // default security attributes
            0,                            // use default stack size  
            _thread_execute_task,         // thread function name
            task_ptr_arr[i],              // argument to thread function 
            0,                            // use default creation flags 
            &(task_ptr_arr[i]->thread_id) // returns the thread identifier
        );
        if (hThreadArr[i] == NULL)
        {
            fprintf(stderr, "Error: failed to create thread, code = %d\n", GetLastError());
            ExitProcess(3);
        }
    }
    WaitForMultipleObjects(dim, hThreadArr, TRUE, INFINITE);
    unsigned long long total_s_count = 0;
    for (int i = 0; i < dim; i++)
    {
        total_s_count += task_ptr_arr[i]->state->s_count;
    }
    double end_time = omp_get_wtime();
    printf("Comput time: %lf\n", end_time - start_time);

    nqp_write_notify_computation_complete(total_s_count);
    nqp_write_wait();
    nqp_write_end();

    for (int i = 0; i < dim; i++)
    {
        CloseHandle(hThreadArr[i]);
    }

    for (int i = 0; i < dim; i++)
        HeapDestroy(task_ptr_arr[i]->task_heap);

    HeapDestroy(global_heap);

    return total_s_count;
}

static DWORD WINAPI _thread_execute_task(LPVOID lpParam) {
    task * t = (task *)lpParam;

    double start_time = omp_get_wtime();
    nqp_iteration(1, t->state);
    double end_time = omp_get_wtime();

    return 0;
}
