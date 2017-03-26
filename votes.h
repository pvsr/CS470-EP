#ifndef _VOTES_H_
#define _VOTES_H_

// #include <gmp.h>
#include <stdint.h>

// generally only used with stv
typedef enum cand_status {
    WINNER, // passed quota
    HOPEFUL,
    LOSER // eliminated
} cand_status_t;

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
    int* cands;
    int num_cands;
} full_vote_t;

// a partial vote for counting, with the current candidate and value
typedef struct counting_vote {
    int cand;
    int value;
    // mpq_t value;
} counting_vote_t;

typedef struct candidate {
    cand_status_t status;
    int location;
} cand_t;

typedef struct list_candidate {
    int votes;
    int seats;
} list_cand_t;

typedef struct electoral_system {
    voting_method_t method;
    int winners;
} electoral_system_t;

int find_max_dbl(double count[], int num_cands);

int find_max_int(int count[], int num_cands, int threshold);

int* count_votes(electoral_system_t vote_sys, cand_t cands[], int num_cands, full_vote_t votes[], uint64_t num_votes, int* num_winners);

counting_vote_t vote_create(full_vote_t full);

#endif /* end of include guard */
