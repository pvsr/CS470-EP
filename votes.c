#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "votes.h"
#include "debug.h"
#include "listpr.h"

int find_max_dbl(double count[], int num_cands) {
    double max_votes = -1;
    int max_index;

    for (int i = 0; i < num_cands; i++) {
        if (count[i] > max_votes) {
            max_votes = count[i];
            max_index = i;
        }
    }

    if (debug) printf("winner is %d with %f\n", max_index, max_votes);
    return max_index;
}

int find_max_int(int count[], int num_cands, int threshold) {
    int max_votes = 0;
    int max_index;

    for (int i = 0; i < num_cands; i++) {
        if (count[i] > threshold) return i;

        if (count[i] > max_votes) {
            max_votes = count[i];
            max_index = i;
        }
    }

    if (debug) printf("winner is %d with %d\n", max_index, max_votes);
    return max_index;
}

// count votes in an fptp election
int count_fptp(int num_cands, vote_t votes[], int num_votes) {
    int count[num_cands];
    memset(count, 0, num_cands * sizeof(int));
    int winner;

    for (int i = 0; i < num_votes; i++) {
        count[votes[i].cand]++;
    }

    winner = find_max_int(count, num_cands, num_votes / 2);

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

    printf("candidate %d wins!\n", winner);
    return winner;
}

int count_votes(electoral_system_t vote_sys, cand_t cands[] __attribute__ ((unused)), int num_cands, vote_t votes[], int num_votes) {
    switch (vote_sys.method) {
        case FPTP:
            return count_fptp(num_cands, votes, num_votes);
        // case PREFERENTIAL:
        case LIST:
            count_list(vote_sys, num_cands, votes, num_votes);
            return 0;
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
