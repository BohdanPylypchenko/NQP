#include "pch.h"

#include "liarr_adjust.h"

unsigned long long adjust_capacity(
	int dim,
	unsigned long long capacity_8,
	unsigned long long capacity_11,
	unsigned long long capacity_15,
	unsigned long long capacity_big
) {
	if (dim <= 8)
		return capacity_8;
	else if (dim <= 11)
		return capacity_11;
	else if (dim <= 15)
		return capacity_15;
	else
		return capacity_big;
}