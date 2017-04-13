#ifndef _RANKED_H_
#define _RANKED_H_

#include <stdbool.h>

#include "votes.h"

typedef struct winner {
    uint32_t index;
    mpq_t votes;
} winner_t;

typedef struct count {
    mpq_t count;
    bool won;
} count_t;

uint32_t* count_stv(electoral_system_t vote_sys, uint32_t num_cands, full_vote_t votes[], uint64_t total_votes);
uint32_t* count_irv(uint32_t num_cands, full_vote_t votes[], uint64_t num_votes);

#endif /* end of include guard */
