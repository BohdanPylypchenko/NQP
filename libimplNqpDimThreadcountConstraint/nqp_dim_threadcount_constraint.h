#pragma once

#define MIN_DIM 4
#define MAX_DIM 26

#define MIN_THREAD_COUNT 1

#define DIM_VALIDATION_ERR_MSG  "Error: dim (%d) should be in [%d;%d]\n"
#define THREAD_COUNT_VALIDATION_ERR_MSG "Error: thread count (%d) should be >= %d\n"
#define DIM_THREADCOUNT_RELATION_VALIDATION_ERR_MSG "Error: thread count (%d) should be <= dim (%d)\n"

int nqp_validate_dim(int dim);

int nqp_validate_threadcount(int thread_count);

int nqp_validate_dim_threadcount_relation(int dim, int thread_count);
