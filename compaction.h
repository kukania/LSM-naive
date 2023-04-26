#pragma once
#include "run.h"

Run* compaction(Level &run_array, bool bf_on, bool indexing_on, std::vector<exact_mapping>& org_map, uint32_t max_lba_range);