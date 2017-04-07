#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "ranked.h"
#include "opts.h"

int find_min_int(int count[], int num_cands) {
    int min_votes = INT_MAX;
    int min_index;

    for (int i = 0; i < num_cands; i++) {
        if (count[i] < min_votes && count[i] > 0) {
            min_votes = count[i];
            min_index = i;
        }
    }

    return min_index;
}

int* count_irv(int num_cands, full_vote_t votes[], int orig_num_votes) {
    int round = 1;
    int loser;
    int count[num_cands];
    int* winner;
    int num_votes = orig_num_votes;

    memset(count, 0, num_cands * sizeof(int));
    winner = malloc(sizeof(int));
    assert(winner != NULL);

    // initial count
    for (int i = 0; i < num_votes; i++) {
        count[votes[i].cands[votes[i].cur]]++;
    }

    while (true) {
        if (pretty) {
            fprintf(output, "<tr><td>round %d votes</td>", round);
            for (int i = 0; i < num_cands; i++) {
                fprintf(output, "<td>%d</td>", count[i]);
            }
            fputs("</tr>", output);

            if (round == 1) {
                fputs("<tr><td>original vote %</td>", output);
                for (int i = 0; i < num_cands; i++) {
                    fprintf(output, "<td>%.2f%%</td>", (double) 100 * count[i] / num_votes);
                }
                fputs("</tr>", output);
            }
        }

        *winner = find_max_int(count, num_cands, num_votes / 2);

        if (count[*winner] >= num_votes / 2) {
            if (debug) printf("%d wins with %d out of %d votes\n", *winner, count[*winner], num_votes);
            break;
        }

        // eliminate last place
        loser = find_min_int(count, num_cands);
        if (debug) printf("eliminating loser %d with %d votes\n", loser, count[loser]);
        count[loser] = 0;
        for (int i = 0; i < num_votes; i++) {
            // skip non-losers
            if (votes[i].cands[votes[i].cur] != loser) {
                continue;
            }
            // exhausted votes are thrown away
            if (votes[i].cur >= votes[i].num_cands - 1) {
                num_votes--;
                continue;
            }
            votes[i].cur++;
            count[votes[i].cands[votes[i].cur]]++;
        }

        round++;
    }

    /*if (pretty) {
        fprintf(output, "<tr><td>round %d votes</td>", round);
        for (int i = 0; i < num_cands; i++) {
            fprintf(output, "<td>%d</td>", count[i]);
        }
        fputs("</tr>", output);
    }*/

    if (pretty) {
        fputs("<tr><td>final vote %</td>", output);
        for (int i = 0; i < num_cands; i++) {
            fprintf(output, "<td>%.2f%%</td>", (double) 100 * count[i] / orig_num_votes);
        }
        fputs("</tr>", output);
    }

    fputs("</table>", output);
    return winner;
}
