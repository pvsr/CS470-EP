#ifndef _LISTPR_H_
#define _LISTPR_H_

#include "votes.h"

int* count_list(electoral_system_t vote_sys, int num_cands, counting_vote_t votes[], int num_votes, int* num_winners);

#endif /* end of include guard */
