#ifndef _VOTES_H_
#define _VOTES_H_

#include <gmp.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum voting_method {
    // plurality systems
    FPTP, // first past the post
    PREFERENTIAL, // instant runoff
    // proportional systems
    LIST, // party list
    STV // single transferable vote
} voting_method_t;

// a full preference vote, with all candidates
typedef struct full_vote {
    uint32_t* cands;
    uint32_t cur;
    uint32_t num_cands;
    mpq_t value;
    bool exhausted;
} full_vote_t;

// a partial vote for counting, with the current candidate and value
typedef struct counting_vote {
    uint32_t cand;
    mpq_t value;
} counting_vote_t;

// the bare minimum vote needed by most counting methods
typedef uint32_t min_vote_t;

typedef struct electoral_system {
    voting_method_t method;
    uint32_t winners;
    uint32_t threshold;
} electoral_system_t;

uint32_t find_max_dbl(double count[], uint32_t num_cands);

uint32_t find_max_int(uint64_t count[], uint32_t num_cands, uint32_t threshold);

uint32_t* count_votes(electoral_system_t vote_sys, uint32_t num_cands, full_vote_t votes[], uint64_t num_votes);

counting_vote_t vote_create(full_vote_t full);

#endif /* end of include guard */
