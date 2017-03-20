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

    printf("candidate %d wins!\n", max_index);
    return max_index;
}

int find_max_list(list_cand_t cands[], int num_cands) {
    int max_votes = -1;
    int max_index = -1;

    for (int i = 0; i < num_cands; i++) {
        if (cands[i].votes > max_votes) {
            max_votes = cands[i].votes;  
            max_index = i;
        } 
    }

    return max_index;
}

void count_list(electoral_system_t vote_sys, int num_cands, vote_t votes[], int num_votes) {
    int count[num_cands];
    memset(count, 0, num_cands * sizeof(int));
    int max_index;
    int seats = vote_sys.winners;
    list_cand_t cands[num_cands];

    for (int i = 0; i < num_votes; i++) {
        count[votes[i].cand]++;
    }

    for (int i = 0; i < num_cands; i++) {
        cands[i] = (list_cand_t) {count[i], 0};
    }

    while (seats > 0) {
        max_index = find_max_list(cands, num_cands);

        cands[max_index].votes = count[max_index] / (cands[max_index].seats + 1);
        cands[max_index].seats++;
        seats--;
    }

    for (int i = 0; i < num_cands; i++) {
        printf("party %d got %d seats!\n", i, cands[i].seats);
    }
}

int count_votes(electoral_system_t vote_sys, cand_t cands[], int num_cands, vote_t votes[], int num_votes) {
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
