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
int count_fptp(int num_cands, counting_vote_t votes[], int num_votes) {
    int count[num_cands];
    memset(count, 0, num_cands * sizeof(int));
    int winner;

    for (int i = 0; i < num_votes; i++) {
        count[votes[i].cand]++;
    }

    winner = find_max_int(count, num_cands, num_votes / 2);

    if (pretty) {
        fputs("<tr><td>votes</td>", output);
        for (int i = 0; i < num_cands; i++) {
            fprintf(output, "<td>%d</td>", count[i]);
        }
        fputs("</tr>", output);
    }

    fputs("</table>", output);

    if(pretty) fprintf(output, "<p>candidate %d wins!</p>", winner + 1);
    printf("candidate %d wins!\n", winner + 1);
    return winner;
}

int count_votes(electoral_system_t vote_sys, cand_t cands[] __attribute__ ((unused)), int num_cands, full_vote_t votes[], int num_votes) {
    counting_vote_t cur_votes[num_votes];
    for (int i = 0; i < num_votes; i++) {
        cur_votes[i] = vote_create(votes[i]);
    }

    switch (vote_sys.method) {
        case FPTP:
            return count_fptp(num_cands, cur_votes, num_votes);
        // case PREFERENTIAL:
        case LIST:
            count_list(vote_sys, num_cands, cur_votes, num_votes);
            return 0;
        // case STV:
        default:
            puts("unimplemented vote vote_sys");
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
