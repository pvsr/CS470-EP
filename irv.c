#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "irv.h"
#include "opts.h"

uint32_t find_min_int(uint64_t count[], uint32_t num_cands) {
    uint32_t min_votes = UINT_MAX;
    uint32_t min_index;

    for (uint32_t i = 0; i < num_cands; i++) {
        if (count[i] < min_votes && count[i] > 0) {
            min_votes = count[i];
            min_index = i;
        }
    }

    return min_index;
}

void count_ranked_votes(full_vote_t votes[], uint64_t total_votes, uint64_t count[], uint64_t* num_valid_votes, uint32_t losers[], uint32_t loser_index) {
    for (uint32_t i = 0; i < total_votes; i++) {
        // skip non-losers
        if (votes[i].cands[votes[i].cur] != losers[loser_index]) {
            continue;
        }

        // move to next cand
        votes[i].cur++;
        for (uint32_t j = 0; j < loser_index + 1 && votes[i].cur < votes[i].num_cands; j++) {
            if (votes[i].cands[votes[i].cur] == losers[j]) {
                votes[i].cur++;
                j = -1;
            }
        }

        // exhausted votes are thrown away
        if (votes[i].cur >= votes[i].num_cands - 1) {
            (*num_valid_votes)--;
            continue;
        }

        // reallocate vote to new choice
        count[votes[i].cands[votes[i].cur]]++;
    }
}

uint32_t* count_irv(uint32_t num_cands, full_vote_t votes[], uint64_t total_votes) {
    int round = 1;
    uint64_t count[num_cands];
    uint32_t losers[num_cands - 2];
    uint32_t loser_index = 0;
    uint32_t* winner;
    uint64_t num_valid_votes = total_votes;

    memset(count, 0, num_cands * sizeof(uint64_t));
    winner = malloc(sizeof(uint32_t));
    assert(winner != NULL);

    // initial count
    for (uint32_t i = 0; i < num_valid_votes; i++) {
        count[votes[i].cands[votes[i].cur]]++;
    }

    while (true) {
        if (pretty) {
            fprintf(output, "<tr>\n<td>round %d votes</td>\n", round);
            for (uint32_t i = 0; i < num_cands; i++) {
                fprintf(output, "<td>%lu</td>\n", count[i]);
            }
            fputs("</tr>\n", output);

            if (round == 1) {
                fputs("<tr>\n<td>original vote %</td>\n", output);
                for (uint32_t i = 0; i < num_cands; i++) {
                    fprintf(output, "<td>%.2f%%</td>\n", (double) 100 * count[i] / num_valid_votes);
                }
                fputs("</tr>\n", output);
            }
        }

        *winner = find_max_int(count, num_cands, num_valid_votes / 2);

        if (count[*winner] >= num_valid_votes / 2) {
            if (debug) printf("%d wins with %lu out of %lu votes\n", *winner, count[*winner], num_valid_votes);
            break;
        }

        // eliminate last place
        losers[loser_index] = find_min_int(count, num_cands);
        if (debug) printf("eliminating loser %d with %lu votes\n",
                          losers[loser_index], count[losers[loser_index]]);
        count[losers[loser_index]] = 0;

        // reallocate and count
        count_ranked_votes(votes, total_votes, count, &num_valid_votes, losers, loser_index);

        round++;
        loser_index++;
    }

    if (pretty) {
        fputs("<tr>\n<td>final vote %</td>\n", output);
        for (uint32_t i = 0; i < num_cands; i++) {
            fprintf(output, "<td>%.2f%%</td>\n", (double) 100 * count[i] / total_votes);
        }
        fputs("</tr>\n</table>\n", output);
    }
    return winner;
}
