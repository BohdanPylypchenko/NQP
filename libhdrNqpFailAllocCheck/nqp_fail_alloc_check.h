#pragma once

#include <stdio.h>
#include <stdlib.h>

#define ALLOC_ERR_MSG "Error: memory allocation failed.\n"
#define nqp_fail_alloc_check(p)\
{\
	if (p == NULL)\
	{\
		fprintf(stderr, ALLOC_ERR_MSG);\
		abort();\
	}\
}
