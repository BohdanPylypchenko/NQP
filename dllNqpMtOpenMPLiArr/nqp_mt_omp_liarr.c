#include "pch.h"

#include "nqp_mt_omp_liarr.h"

#include "nqp_mt.h"
#include "nqp_mt_liarr.h"
#include "nqp_dim_threadcount_constraint.h"

__declspec(dllexport) unsigned long long nqp_mt_omp_liarr(int dim, int thread_count)
{
	if ((nqp_validate_dim(dim) != 0) ||
		(nqp_validate_threadcount(thread_count) != 0))
	{
		return -1;
	}
	else
	{
		return nqp_mt_liarr(dim, thread_count);
	}
}