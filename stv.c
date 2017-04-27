#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <mpi.h>
#include <limits.h>

#include "stv.h"
#include "opts.h"
#include "comms.h"

static int round = 1;

int cmp_eliminated (const void* a, const void* b) {
    eliminated_t *ae, *be;
    ae = (eliminated_t*)a;
    be = (eliminated_t*)b;

    if (ae->won && be->won)
        return mpq_cmp(((eliminated_t*)a)->votes, ((eliminated_t*)b)->votes);
    else if (ae->won && !be->won)
        return 1;
    else if (!ae->won && be->won)
        return -1;
    else return 0;
}

void reset_count(full_vote_t votes[], uint64_t total_votes, eliminated_t eliminated[], uint32_t* eliminated_index) {
    // clear provisional winners
    for (uint32_t j = 0; j < *eliminated_index; j++)
    {
        if (eliminated[j].won)
            mpq_clear(eliminated[j].votes);
    }

    // shift excluded candidates
    int j = 0;
    for (uint32_t i = 0; i < *eliminated_index; i++)
    {
        if (!eliminated[i].won)
        {
            eliminated[j++] = eliminated[i];
        }
    }

    *eliminated_index = j;


    for (uint64_t i = 0; i < total_votes; i++) {
        // move to first valid cand
        votes[i].cur = 0;
        for (uint32_t j = 0; j < *eliminated_index && votes[i].cur < votes[i].num_cands; j++) {
            if (votes[i].cands[votes[i].cur] == eliminated[j].index) {
                votes[i].cur++;
                j = -1;
            }
        }

        // vote is totally exhausted, i.e. only has prefs for excluded cands
        if (votes[i].cur == votes[i].num_cands)
            votes[i].exhausted = true;
    }
}

uint32_t find_min_count_t(count_t count[], uint32_t num_cands, eliminated_t eliminated[], uint32_t eliminated_index) {
    mpq_t min_votes;
    uint32_t min_index;

    mpq_init(min_votes);
    mpq_set_ui(min_votes, ULONG_MAX, 1);

    for (uint32_t i = 0; i < num_cands; i++) {
        bool present = false;
        for (uint32_t j = 0; j < eliminated_index && !present; j++)
        {
            present = i == eliminated[j].index;
        }

        if (!present && mpq_cmp(count[i].count, min_votes) < 0 && mpq_cmp_ui(count[i].count, 0, 1) > 0) {
            mpq_set(min_votes, count[i].count);
            min_index = i;
        }
    }

    mpq_clear(min_votes);

    return min_index;
}

void check_for_winners(uint64_t total_votes, count_t count[], uint32_t num_cands, uint32_t* remaining_winners, eliminated_t* eliminated, uint32_t* eliminated_index, mpq_t quota, bool before_transfer) {
    static bool percent_printed = true;

    for (uint32_t i = 0; i < num_cands && *remaining_winners > 0; i++) {
        if (!count[i].won && mpq_cmp(count[i].count, quota) >= 0)
        {
            if (debug) printf("candidate %d passes quota %f with %f votes\n", i,
                              mpq_get_d(quota), mpq_get_d(count[i].count));
            count[i].won = true;

            eliminated[*eliminated_index].index = i;
            mpq_init(eliminated[*eliminated_index].votes);
            mpq_set(eliminated[*eliminated_index].votes, count[i].count);
            eliminated[*eliminated_index].won = true;
            (*eliminated_index)++;
            assert(*eliminated_index <= num_cands);
            (*remaining_winners)--;
        }
    }

    if (pretty) {
        fprintf(output, "<tr>\n<td>round %d votes%s</td>\n", round, before_transfer ? "" : ", after surplus transfer");
        for (uint32_t i = 0; i < num_cands; i++) {
            // find winners so we can bold them
            bool won = false;
            for (uint32_t j = 0; j < *eliminated_index && !won; j++)
            {
                won = i == eliminated[j].index && eliminated[j].won;
            }
            if (won)
                fprintf(output, "<td><b>%.2f</b></td>\n", mpq_get_d(count[i].count));
            else
                fprintf(output, "<td>%.2f</td>\n", mpq_get_d(count[i].count));
        }
        fputs("</tr>\n", output);

        if (percent_printed) {
            percent_printed = false;
            fputs("<tr>\n<td>original vote %</td>\n", output);
            for (uint32_t i = 0; i < num_cands; i++) {
                fprintf(output, "<td>%.2f%%</td>\n", 100 * mpq_get_d(count[i].count) / total_votes);
            }
            fputs("</tr>\n", output);
        }

        if (debug) printf("end of round %d\n", round);
    }
}

void redistribute_surplus(full_vote_t votes[], uint32_t total_votes, count_t count[], eliminated_t eliminated[], uint32_t eliminated_index) {
    // loop through every vote to find votes to reassign
    // note that we are only reassigning winners right now
    // may be a place to look for optimizations
    for (uint32_t i = 0; i < total_votes; i++) {
        bool won = false;
        uint32_t cand;

        // exhausted votes
        if (votes[i].cur >= votes[i].num_cands - 1) continue;
        // TODO optional? Any ballot paper that does not express a valid
        // preference for a continuing candidate greater than the preference
        // allocated to the candidates whose surplus is to be distributed shall
        // be set aside and declared exhausted-with-value and its value added to
        // the total value of exhausted ballot papers recorded for the relevant
        // transaction in the count. Exhausted votes that form part of a
        // candidates surplus remain in the count and form part of the initial
        // candidate’s Total Vote and surplus.

        // identify votes for winners
        for (uint32_t j = 0; j < eliminated_index; j++)
        {
            won = votes[i].cands[votes[i].cur] == eliminated[j].index && eliminated[j].won;

            // end loop
            if (won) {
                cand = j;
                break;
            }
        }

        // ignore non-winning votes
        if (!won) continue;

        // multiply the value of a winning vote by its cand's surplus transfer value
        mpq_mul(votes[i].value, votes[i].value, eliminated[cand].votes);
        mpq_canonicalize(votes[i].value);

        // distribution of candidate's surplus votes
        // move to next valid cand
        votes[i].cur++;
        for (uint32_t j = 0; j < eliminated_index && votes[i].cur < votes[i].num_cands; j++) {
            if (votes[i].cands[votes[i].cur] == eliminated[j].index) {
                votes[i].cur++;
                j = -1;
            }
        }

        // reallocate votes
        mpq_add(count[votes[i].cands[votes[i].cur]].count,
                count[votes[i].cands[votes[i].cur]].count, votes[i].value);
        mpq_canonicalize(count[votes[i].cands[votes[i].cur]].count);
    }
}

void handle_surplus(full_vote_t votes[], uint64_t total_votes, uint32_t num_cands, count_t count[], eliminated_t eliminated[], uint32_t eliminated_index, mpq_t quota) {
    if (eliminated_index > 1) {
        qsort(eliminated, eliminated_index, sizeof(eliminated_t), cmp_eliminated);
    }

    // calculate surplus transfer value
    for (uint32_t i = 0; i < eliminated_index; i++) {
        if (!eliminated[i].won) continue;
        // surplus value = cand's votes - quota
        mpq_sub(eliminated[i].votes, count[eliminated[i].index].count, quota);
        mpq_canonicalize(eliminated[i].votes);

        // surplus transfer value = surplus value / cand's votes
        mpq_div(eliminated[i].votes, eliminated[i].votes, count[eliminated[i].index].count);
        mpq_canonicalize(eliminated[i].votes);
    }

    redistribute_surplus(votes, total_votes, count, eliminated, eliminated_index);

    // allocate quota to winners
    for (uint32_t i = 0; i < num_cands; i++) {
        bool win = false;
        for (uint32_t j = 0; j < eliminated_index && !win; j++)
            win = i == eliminated[j].index && eliminated[j].won;

        if (!win) continue;

        mpq_set(count[i].count, quota);
    }
}

uint32_t* count_stv(electoral_system_t vote_sys, uint32_t num_cands, full_vote_t votes[], uint64_t local_votes) {
    if (vote_sys.winners > num_cands) {
        puts("The number of available seats must be less than the number of candidates!");
        exit(1);
    }

    eliminated_t current;
    eliminated_t eliminated[num_cands];
    uint32_t eliminated_index = 0;
    uint32_t remaining_winners = vote_sys.winners;
    mpq_t quota;
    uint64_t num_valid_votes = local_votes;
    uint64_t global_valid_votes = local_votes;
    uint64_t global_votes = local_votes;
    uint32_t* result;
    count_t count[num_cands];
    count_t global_count[num_cands];
    count_t tmp_global_count[num_cands];
    long double transfer_count[num_cands];
    uint32_t int_count_len = num_cands + 2;
    uint64_t int_count[int_count_len];

    result = malloc(vote_sys.winners * sizeof(uint32_t));
    assert(result != NULL);

    while (true) {
        memset(int_count, 0, int_count_len * sizeof(uint64_t));
        memset(count, 0, num_cands * sizeof(count_t));
        memset(global_count, 0, num_cands * sizeof(count_t));
        memset(tmp_global_count, 0, num_cands * sizeof(count_t));
        memset(transfer_count, 0, num_cands * sizeof(double));
        remaining_winners = vote_sys.winners;

        // initial distribution of preferences and calculation of quota
        for (uint32_t i = 0; i < local_votes; i++) {
            if (votes[i].exhausted) {
                num_valid_votes--;
                continue;
            }

            int_count[votes[i].cands[votes[i].cur]]++;
        }

        // convert count to fractional representation
        for (uint32_t i = 0; i < num_cands; i++) {
            mpq_init(count[i].count);
            mpq_set_ui(count[i].count, int_count[i], 1);
        }

        int_count[int_count_len - 2] = local_votes;
        int_count[int_count_len - 1] = num_valid_votes;

        if (num_procs > 1) {
            MPI_Allreduce(MPI_IN_PLACE, int_count, int_count_len,
                    MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);

            global_votes = int_count[int_count_len - 2];
            global_valid_votes = int_count[int_count_len - 1];
            for (uint32_t i = 0; i < num_cands; i++) {
                mpq_init(tmp_global_count[i].count);
                mpq_init(global_count[i].count);
                mpq_set_ui(global_count[i].count, int_count[i], 1);
            }
        }

        mpq_init(quota);

        // truncation is needed anyway so integer division is right
        if (num_procs > 1)
            mpq_set_ui(quota, 1 + global_valid_votes / (1 + vote_sys.winners), 1);
        else
            mpq_set_ui(quota, 1 + num_valid_votes / (1 + vote_sys.winners), 1);

        if (pretty) {
            fputs("<tr>\n<td>current quota</td>\n", output);
            fprintf(output, "<td colspan=%d>%.0f</td>\n", num_cands, mpq_get_d(quota));
            fputs("</tr>\n", output);
        }

        if (debug && pid == 0) printf("quota: %f\n", mpq_get_d(quota));

        // provisional declaration of elected candidates
        if (num_procs > 1)
            check_for_winners(global_votes, global_count, num_cands, &remaining_winners, eliminated, &eliminated_index, quota, true);
        else
            check_for_winners(local_votes, count, num_cands, &remaining_winners, eliminated, &eliminated_index, quota, true);

        // early end
        if (remaining_winners == 0) {
            if (debug && pid == 0) puts("all seats filled by quota");
            break;
        }

        // at least one new winner
        if (remaining_winners != vote_sys.winners) {
            if (num_procs > 1) {
                for (uint32_t i = 0; i < num_cands; i++) {
                    mpq_set(tmp_global_count[i].count, global_count[i].count);
                }

                handle_surplus(votes, local_votes, num_cands, tmp_global_count, eliminated, eliminated_index, quota);

                for (uint32_t i = 0; i < num_cands; i++) {
                    mpq_sub(tmp_global_count[i].count, tmp_global_count[i].count, global_count[i].count);
                    mpq_canonicalize(tmp_global_count[i].count);
                    transfer_count[i] = mpq_get_d(tmp_global_count[i].count);
                }

                // some loss of precision is invevitable
                MPI_Allreduce(MPI_IN_PLACE, transfer_count, num_cands,
                        MPI_LONG_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

                for (uint32_t i = 0; i < num_cands; i++) {
                    bool won = false;
                    for (uint32_t j = 0; j < eliminated_index && !won; j++)
                        won = i == eliminated[j].index && eliminated[j].won;

                    // TODO technically this duplicates the quota allocation in
                    // handle_surplus. may need significant refactoring to fix
                    if (won) {
                        mpq_set(global_count[i].count, quota);
                    }
                    else {
                        mpq_set_d(tmp_global_count[i].count, transfer_count[i]);
                        mpq_add(global_count[i].count, global_count[i].count, tmp_global_count[i].count);
                        mpq_canonicalize(global_count[i].count);
                    }
                }

                check_for_winners(global_votes, global_count, num_cands, &remaining_winners, eliminated, &eliminated_index, quota, false);
            }
            else {
                handle_surplus(votes, local_votes, num_cands, count, eliminated, eliminated_index, quota);
                check_for_winners(local_votes, count, num_cands, &remaining_winners, eliminated, &eliminated_index, quota, false);
            }
        }

        // TODO (optional) bulk exclusion
        current.index = find_min_count_t(count, num_cands, eliminated, eliminated_index);
        current.won = false;
        eliminated[eliminated_index] = current;
        if (debug && pid == 0) printf("eliminated candidate %d with %.2f votes\n",
                eliminated[eliminated_index].index,
                mpq_get_d(count[eliminated[eliminated_index].index].count));
        eliminated_index++;
        assert(eliminated_index <= num_cands);

        if (num_procs > 1) {
            for (uint32_t i = 0; i < num_cands; i++) {
                mpq_clear(global_count[i].count);
                mpq_clear(tmp_global_count[i].count);
            }
        }

        // number of seats remaining == number of candidates remaining?
        if (num_cands - eliminated_index == remaining_winners)
        {
            if (debug && pid == 0) puts("remaining seats == remaining candidates");
            break;
        }

        if (debug && pid == 0) puts("resetting count");

        reset_count(votes, local_votes, eliminated, &eliminated_index);

        mpq_clear(quota);

        for (uint32_t i = 0; i < num_cands; i++) {
            mpq_clear(count[i].count);
        }

        for (uint32_t i = 0; i < local_votes; i++) {
            mpq_set_ui(votes[i].value, 1, 1);
        }

        round++;
    }

    uint32_t result_idx = 0;
    for (uint32_t i = 0; i < eliminated_index; i++) {
        if (eliminated[i].won)
            result[result_idx++] = eliminated[i].index;
    }

    // assign candidates still in play to winners
    for (uint32_t i = 0; result_idx < vote_sys.winners && i < num_cands; i++)
    {
        bool present;
        for (uint32_t j = 0; j < eliminated_index; j++)
        {
            present = i == eliminated[j].index;
            if (present) break;
        }
        if (present) continue;
        result[result_idx++] = i;
    }

    mpq_clear(quota);

    for (uint32_t i = 0; i < num_cands; i++) {
        mpq_clear(count[i].count);
    }

    for (uint32_t i = 0, j = 0; i < vote_sys.winners && j < eliminated_index; i++) {
        if (eliminated[j].won)
            mpq_clear(eliminated[j++].votes);
    }

    if (pretty) fputs("</table>\n", output);

    return result;
}
