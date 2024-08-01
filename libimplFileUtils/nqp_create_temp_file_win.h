#pragma once

#include "WinapiConfig.h"

HANDLE create_temp_file_win(
	unsigned long long primary_id,
	unsigned long long secondary_id
);
