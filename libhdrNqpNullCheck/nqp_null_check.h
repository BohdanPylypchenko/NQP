#pragma once

#include <stdio.h>
#include <stdlib.h>

#define ALLOC_ERR_MSG "Error: null check failed, function %s\n"

#define nqp_null_check(p)\
if (p == NULL)\
{\
	fprintf(stderr, ALLOC_ERR_MSG, __func__);\
	abort();\
}\
