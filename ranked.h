#ifndef _RANKED_H_
#define _RANKED_H_

#include "votes.h"

typedef struct winner {
    unsigned int index;
    mpq_t votes;
} winner_t;

unsigned int* count_stv(electoral_system_t vote_sys, int num_cands, full_vote_t votes[], int total_votes);
unsigned int* count_irv(int num_cands, full_vote_t votes[], int num_votes);

#endif /* end of include guard */
