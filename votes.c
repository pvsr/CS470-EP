#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "votes.h"
#include "opts.h"
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

    if (debug) printf("max is %d with %f\n", max_index, max_votes);
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

    if (debug) printf("max is %d with %d\n", max_index, max_votes);
    return max_index;
}

// count votes in an fptp election
int* count_fptp(int num_cands, counting_vote_t votes[], int num_votes) {
    int count[num_cands];
    memset(count, 0, num_cands * sizeof(int));
    int* winner;
    winner = malloc(sizeof(int));
    assert(winner != NULL);

    for (int i = 0; i < num_votes; i++) {
        count[votes[i].cand]++;
    }

    *winner = find_max_int(count, num_cands, num_votes / 2);

    if (pretty) {
        fputs("<tr><td>votes</td>", output);
        for (int i = 0; i < num_cands; i++) {
            fprintf(output, "<td>%d</td>", count[i]);
        }
        fputs("</tr>", output);

        fputs("<tr><td>vote %</td>", output);
        for (int i = 0; i < num_cands; i++) {
            fprintf(output, "<td>%.2f%%</td>", (double) 100 * count[i] / num_votes);
        }
        fputs("</tr>", output);
    }

    if (pretty) fputs("</table>", output);
    return winner;
}

int* count_votes(electoral_system_t vote_sys, cand_t cands[] __attribute__ ((unused)), int num_cands, full_vote_t votes[], uint64_t num_votes, int* num_winners) {
    int* result;
    counting_vote_t* cur_votes;

    cur_votes = malloc(num_votes * sizeof(counting_vote_t));
    assert(cur_votes != NULL);

    for (uint64_t i = 0; i < num_votes; i++) {
        cur_votes[i] = vote_create(votes[i]);
    }

    switch (vote_sys.method) {
        case FPTP:
            *num_winners = 1;
            result = count_fptp(num_cands, cur_votes, num_votes);
            free(cur_votes);
            return result;
        // case PREFERENTIAL:
        case LIST:
            result = count_list(vote_sys, num_cands, cur_votes, num_votes, num_winners);
            free(cur_votes);
            return result;
        // case STV:
        default:
            puts("unimplemented vote method");
            exit(1);
    }
}

counting_vote_t vote_create(full_vote_t full) {
    counting_vote_t result;
    result.cand = full.cands[0];
    result.value = 1;
    // mpq_init(result.value);
    // no need for canonicalization
    // mpq_set_ui(result.value, 1, 1);
    return result;
}
