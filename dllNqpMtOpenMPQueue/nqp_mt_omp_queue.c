#include "pch.h"

#include "nqp_mt_omp_queue.h"

#include "nqp_mt.h"
#include "nqp_mt_queue.h"
#include "nqp_dim_threadcount_constraint.h"

__declspec(dllexport) unsigned long long nqp_mt_omp_queue(int dim, int thread_count)
{
	if ((nqp_validate_dim(dim) != 0) ||
		(nqp_validate_threadcount(thread_count) != 0))
	{
		return -1;
	}
	else
	{
		return nqp_mt_queue(dim, thread_count);
	}
}