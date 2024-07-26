#pragma once

#include "WinapiConfig.h"

void split_dwordlong_dword_dword(
    DWORDLONG src,
    LPDWORD high,
    LPDWORD low
);

DWORDLONG concat_dword_dword(
    DWORD high,
    DWORD low
);
