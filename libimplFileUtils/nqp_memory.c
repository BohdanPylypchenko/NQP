#include "pch.h"

#include "nqp_memory.h"

#include "WinapiConfig.h"

static const double MEMORY_SAFETY_COEFFICIENT = 0.75;

unsigned long long get_available_physical_memory_bytes(void)
{
    MEMORYSTATUSEX memorystatusex;
    memorystatusex.dwLength = sizeof(memorystatusex);

    if (GlobalMemoryStatusEx(&memorystatusex))
    {
        return (unsigned long long)((double)memorystatusex.ullAvailPhys * MEMORY_SAFETY_COEFFICIENT);
    }
    else
    {
        return -1;
    }
}