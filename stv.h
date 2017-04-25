#ifndef _STV_H_
#define _STV_H_

#include <stdbool.h>

#include "votes.h"

typedef struct eliminated {
    uint32_t index;
    mpq_t votes;
    bool won;
} eliminated_t;

typedef struct count {
    mpq_t count;
    bool won;
} count_t;

uint32_t* count_stv(electoral_system_t vote_sys, uint32_t num_cands, full_vote_t votes[], uint64_t total_votes);

#endif /* end of include guard */
