#pragma once

#include "nqp_io_winbuffer.h"
#include "WinapiConfig.h"

nqp_winbuffer * nqp_winbuffer_new(
	unsigned long long winbuffer_id,
	HANDLE file, HANDLE heap,
	size_t size_in_bytes
);

void nqp_winbuffer_append(nqp_winbuffer * winbuffer, int dim, int * solution);

void nqp_winbuffer_flush(nqp_winbuffer * winbuffer);
