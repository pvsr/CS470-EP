#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "votes.h"
#include "debug.h"

int count_fptp(int num_cands, vote_t votes[], int num_votes) {
    int count[num_cands];
    memset(count, 0, num_cands * sizeof(int));
    int max_votes = -1;
    int max_index = -1;

    for (int i = 0; i < num_votes; i++) {
        count[votes[i].cand]++;
    }

    for (int i = 0; i < num_cands; i++) {
        // if one cand gets over 50%, they win outright
        if (count[i] > num_votes / 2) return i;
        
        if (count[i] > max_votes) {
            max_votes = count[i];  
            max_index = i;
        } 
    }

    if (debug) {
        printf("|");
        for (int i = 1; i <= num_cands; i++) {
            printf("%4d   |", i);
        }
        printf("\n|");
        for (int i = 0; i < num_cands; i++) {
            printf("%4d   |", count[i]);
        }
        puts("");
    }

    return max_index;
}

int count_votes(electoral_system_t vote_sys, cand_t cands[], int num_cands, vote_t votes[], int num_votes) {
    switch (vote_sys.method) {
        case FPTP:
            return count_fptp(num_cands, votes, num_votes);
        // case PREFERENTIAL:
        // case LIST:
        // case STV:
        default:
            puts("unimplemented vote vote_sys");
            exit(1);
    }
}

vote_t vote_create(int cand) {
    vote_t result;
    result.cand = cand;
    mpq_init(result.value);
    // no need for canonicalization
    mpq_set_ui(result.value, 1, 1);
    return result;
}
