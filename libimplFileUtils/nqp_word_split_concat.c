#include "pch.h"

#include "nqp_word_split_concat.h"

#include "WinapiConfig.h"

void split_dwordlong_dword_dword(
    DWORDLONG src,
    LPDWORD high,
    LPDWORD low
) {
    *low = src & 0xFFFFFFFF;
    *high = (src >> 32) & 0xFFFFFFFF;
}

DWORDLONG concat_dword_dword(
    DWORD high,
    DWORD low
) {
    return ((DWORDLONG)high << 32) | low;
}