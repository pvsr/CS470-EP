#ifndef _IRV_H_
#define _IRV_H_

#include <stdbool.h>

#include "votes.h"

uint32_t* count_irv(uint32_t num_cands, full_vote_t votes[], uint64_t num_votes);

#endif /* end of include guard */
