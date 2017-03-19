#ifndef _VOTES_H_
#define _VOTES_H_

#include <gmp.h>

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

typedef struct vote {
    int cand;
    mpq_t value;
} vote_t;

typedef struct candidate {
    cand_status_t status;
    int location;
} cand_t;

typedef struct electoral_system {
    voting_method_t method;
    int winners;
} electoral_system_t;

// read votes from a file
// TODO vote file format
void read_votes(char* filename);

int count_votes(electoral_system_t method, cand_t cands[], int num_cands, vote_t votes[], int num_votes);

vote_t vote_create(int cand);

#endif /* end of include guard */
