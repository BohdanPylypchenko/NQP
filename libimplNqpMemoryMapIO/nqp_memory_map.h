#pragma once

#include "WinapiConfig.h"

typedef enum
{
    NQP_MEMORY_MAP_OVERFLOW,
    NQP_MEMORY_MAP_SUCCESS
} nqp_memory_map_status;

typedef struct
{
    unsigned long long id;
    HANDLE file;
    LPBYTE mbegin;
    LPBYTE mcurrent;
    size_t mwritten;
    size_t memory_size_in_bytes;
} nqp_memory_map;

nqp_memory_map * nqp_memory_map_new(
    unsigned long long id,
    HANDLE heap,
    HANDLE file,
    size_t memory_size_in_bytes,
    int nqp_dim
);

void nqp_memory_map_free(HANDLE heap, nqp_memory_map * memory_map);

nqp_memory_map_status nqp_memory_map_try_write(
    nqp_memory_map * memory_map,
    LPBYTE src,
    size_t src_size_in_bytes
);
