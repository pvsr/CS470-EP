#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "votes.h"
#include "opts.h"
#include "listpr.h"
#include "stv.h"
#include "irv.h"
#include "fptp.h"

uint32_t find_max_dbl(double count[], uint32_t num_cands) {
    double max_votes = -1;
    uint32_t max_index;

    for (uint32_t i = 0; i < num_cands; i++) {
        if (count[i] > max_votes) {
            max_votes = count[i];
            max_index = i;
        }
    }

    if (debug) printf("max is %d with %f\n", max_index, max_votes);
    return max_index;
}

uint32_t* count_votes(electoral_system_t vote_sys, uint32_t num_cands, full_vote_t votes[], uint64_t num_votes) {
    uint32_t* result;
    min_vote_t* cur_votes;

    if (vote_sys.method == FPTP || vote_sys.method == LIST) {
        cur_votes = malloc(num_votes * sizeof(min_vote_t));
        assert(cur_votes != NULL);

        for (uint64_t i = 0; i < num_votes; i++) {
            cur_votes[i] = votes[i].cands[0];
        }
    }
    else if (vote_sys.method == STV) {
        // single-winner stv is equivalent to irv
        if (vote_sys.winners == 1) {
            vote_sys.method = PREFERENTIAL;
        }
        else {
            for (uint64_t i = 0; i < num_votes; i++) {
                mpq_init(votes[i].value);
                mpq_set_ui(votes[i].value, 1, 1);
            }
        }
    }

    switch (vote_sys.method) {
        case FPTP:
            result = count_fptp(num_cands, cur_votes, num_votes);
            free(cur_votes);
            return result;
        case PREFERENTIAL:
            return count_irv(num_cands, votes, num_votes);
        case LIST:
            result = count_list(vote_sys, num_cands, cur_votes, num_votes);
            free(cur_votes);
            return result;
        case STV:
            return count_stv(vote_sys, num_cands, votes, num_votes);
        default:
            puts("unimplemented vote method");
            exit(1);
    }
}

counting_vote_t vote_create(full_vote_t full) {
    counting_vote_t result;
    result.cand = full.cands[0];
    mpq_init(result.value);
    // no need for canonicalization
    mpq_set_ui(result.value, 1, 1);
    return result;
}
