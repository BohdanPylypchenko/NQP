#include "pch.h"

#include "nqp_memory_map.h"

#include "WinapiConfig.h"
#include "nqp_null_check.h"

#include <string.h>
#include <stdio.h>

nqp_memory_map * nqp_memory_map_new(
    unsigned long long id,
    HANDLE heap,
    HANDLE file,
    size_t memory_size_in_bytes,
    int nqp_dim
) {
    nqp_memory_map * memory_map = HeapAlloc(heap, 0, sizeof(nqp_memory_map));
    nqp_null_check(memory_map);
    
    ULARGE_INTEGER uli;
    uli.QuadPart = memory_size_in_bytes;

    HANDLE file_mapping = CreateFileMapping(
        file,
        NULL,
        PAGE_READWRITE,
        uli.HighPart,
        uli.LowPart,
        NULL
    );
    nqp_null_check(file_mapping);

    LPBYTE mbegin = MapViewOfFile(
        file_mapping,
        FILE_MAP_WRITE,
        0, 0,
        memory_size_in_bytes
    );
    nqp_null_check(mbegin)
    
    CloseHandle(file_mapping);

    memory_map->id = id;
    memory_map->file = file;
    memory_map->mbegin = mbegin;
    memory_map->mcurrent = mbegin + sizeof(int) + sizeof(unsigned long long);
    memory_map->mwritten = 0;
    memory_map->memory_size_in_bytes = memory_size_in_bytes;

	memcpy(memory_map->mbegin, &nqp_dim, sizeof(int));

    return memory_map;
}

void nqp_memory_map_free(HANDLE heap, nqp_memory_map * memory_map)
{
    if (!FlushViewOfFile(
        memory_map->mbegin,
        memory_map->mcurrent - memory_map->mbegin
    )) {
        fprintf(stderr, "Error: failed to flush memory map %llu\n", memory_map->id);
    }

    UnmapViewOfFile(memory_map->mbegin);

    HeapFree(heap, 0, memory_map);
}

nqp_memory_map_status nqp_memory_map_try_write(
    nqp_memory_map * memory_map,
    LPBYTE src,
    size_t src_size_in_bytes
) {
    if (memory_map->mwritten + src_size_in_bytes > memory_map->memory_size_in_bytes)
    {
        return NQP_MEMORY_MAP_OVERFLOW;
    }
    else
    {
        memcpy(memory_map->mcurrent, src, src_size_in_bytes);

        memory_map->mcurrent += src_size_in_bytes;
        memory_map->mwritten += src_size_in_bytes;

        return NQP_MEMORY_MAP_SUCCESS;
    }
}
