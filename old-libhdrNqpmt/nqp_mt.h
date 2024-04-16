#pragma once

#include "nqp_io.h"

unsigned long long nqp_mt(
	int dim,
	int thread_count,
	nqp_writer ** writer_arr,
	nqp_start_args * start_args
);
