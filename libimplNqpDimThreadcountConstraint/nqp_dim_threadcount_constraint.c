#include "pch.h"

#include "nqp_dim_threadcount_constraint.h"

#include <stdio.h>

int nqp_validate_dim(int dim)
{
    if (dim < MIN_DIM || dim > MAX_DIM)
    {
        fprintf(stderr, DIM_VALIDATION_ERR_MSG, dim, MIN_DIM, MAX_DIM);
        return 1;
    }
    else
    {
        return 0;
    }
}

int nqp_validate_threadcount(int thread_count)
{
    if (thread_count < MIN_THREAD_COUNT)
    {
        fprintf(stderr, THREAD_COUNT_VALIDATION_ERR_MSG, thread_count, MIN_THREAD_COUNT);
        return 1;
    }
    else
    {
        return 0;
    }
}

int nqp_validate_dim_threadcount_relation(int dim, int thread_count)
{
    if (thread_count > dim)
    {
        fprintf(stderr, DIM_THREADCOUNT_RELATION_VALIDATION_ERR_MSG, thread_count, dim);
        return 1;
    }
    else
    {
        return 0;
    }
}