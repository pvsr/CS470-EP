#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <mpi.h>

#include "irv.h"
#include "opts.h"
#include "comms.h"

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

// reallocate and recount votes at the end of a round
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

// count votes in an irv election
uint32_t* count_irv(uint32_t num_cands, full_vote_t votes[], uint64_t total_votes) {
    int round = 1;
    int64_t current;
    uint32_t count_len = num_cands + 2;
    uint64_t count[count_len];
    uint64_t global_count[count_len];
    uint32_t losers[num_cands - 2];
    uint32_t loser_index = 0;
    uint32_t* winner;
    uint64_t num_valid_votes = total_votes;
    uint64_t global_valid_votes = total_votes;
    uint64_t global_votes = total_votes;

    memset(count, 0, count_len * sizeof(uint64_t));
    memset(global_count, 0, count_len * sizeof(uint64_t));
    winner = malloc(sizeof(uint32_t));
    assert(winner != NULL);

    // initial count
    if (num_procs > 1) {
        for (uint32_t i = 0; i < num_valid_votes; i++) {
            count[votes[i].cands[votes[i].cur]]++;
        }
    }
    else {
        for (uint32_t i = 0; i < num_valid_votes; i++) {
            global_count[votes[i].cands[votes[i].cur]]++;
        }
    }

    if (num_procs > 1) {
        count[count_len - 2] = num_valid_votes;
        count[count_len - 1] = total_votes;
        MPI_Reduce(count, global_count, count_len,
                MPI_UNSIGNED_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
        if (pid == 0) {
            global_valid_votes = global_count[count_len - 1];
            global_votes = global_count[count_len - 2];
        }
    }

    while (true) {
        if (pretty) {
            fprintf(output, "<tr>\n<td>round %d votes</td>\n", round);
            for (uint32_t i = 0; i < num_cands; i++) {
                fprintf(output, "<td>%lu</td>\n", global_count[i]);
            }
            fputs("</tr>\n", output);

            if (round == 1) {
                fputs("<tr>\n<td>original vote %</td>\n", output);
                for (uint32_t i = 0; i < num_cands; i++) {
                    fprintf(output, "<td>%.2f%%</td>\n", (double) 100 * global_count[i] / global_valid_votes);
                }
                fputs("</tr>\n", output);
            }
        }

        if (pid == 0) {
            *winner = find_max_int(global_count, num_cands, global_valid_votes / 2);

            if (global_count[*winner] >= global_valid_votes / 2) {
                if (debug) printf("%d wins with %lu out of %lu votes\n", *winner, global_count[*winner], global_valid_votes);
                if (num_procs > 1) {
                    // notify other threads that it's over
                    current = -1;
                    if (debug) printf("rank %d: %d bcast\n", pid, round);
                    MPI_Bcast(&current, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
                }
                break;
            }

            // eliminate last place
            losers[loser_index] = find_min_int(global_count, num_cands);
            if (debug) printf("eliminating loser %d with %lu votes\n",
                    losers[loser_index], global_count[losers[loser_index]]);
        }

        if (num_procs > 1) {
            if (pid == 0) current = losers[loser_index];
            if (debug) printf("rank %d: %d bcast\n", pid, round);
            MPI_Bcast(&current, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
            if (current < 0) break;
            if (pid != 0) losers[loser_index] = current;
        }


        // reallocate and count
        if (num_procs > 1) {
            count[losers[loser_index]] = 0;
            count_ranked_votes(votes, total_votes, count, &num_valid_votes, losers, loser_index);
        }
        else {
            global_count[losers[loser_index]] = 0;
            count_ranked_votes(votes, total_votes, global_count, &global_valid_votes, losers, loser_index);
        }

        // sum count and total_votes
        if (num_procs > 1) {
            count[count_len - 2] = num_valid_votes;
            if (debug) printf("rank %d: %d reduce\n", pid, round);
            MPI_Reduce(count, global_count, count_len - 1,
                    MPI_UNSIGNED_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
            if (pid == 0) global_valid_votes = global_count[count_len - 2];
        }

        round++;
        loser_index++;
    }

    if (pretty) {
        fputs("<tr>\n<td>final vote %</td>\n", output);
        for (uint32_t i = 0; i < num_cands; i++) {
            fprintf(output, "<td>%.2f%%</td>\n", (double) 100 * global_count[i] / global_votes);
        }
        fputs("</tr>\n</table>\n", output);
    }
    return winner;
}
