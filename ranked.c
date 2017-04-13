#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "ranked.h"
#include "opts.h"

int cmp_winners (const void* a, const void* b)
{
    return mpq_cmp(((winner_t*)a)->votes, ((winner_t*)b)->votes);
}

uint32_t find_min_count_t(count_t count[], uint32_t num_cands) {
    mpq_t min_votes;
    uint32_t min_index;

    mpq_init(min_votes);
    mpq_set_ui(min_votes, ULONG_MAX, 1);

    for (uint32_t i = 0; i < num_cands; i++) {
        if (mpq_cmp(count[i].count, min_votes) < 0 && mpq_cmp_ui(count[i].count, 0, 1) > 0) {
            mpq_set(min_votes, count[i].count);
            min_index = i;
        }
    }

    mpq_clear(min_votes);

    return min_index;
}

void check_for_winners(count_t count[], uint32_t num_cands, uint32_t* remaining_winners, winner_t* winners, uint32_t* winner_index, uint32_t eliminated[], uint32_t* eliminated_index, mpq_t quota) {
    for (uint32_t i = 0; i < num_cands && *remaining_winners > 0; i++) {
        // TODO don't count previous winners???
        if (!count[i].won && mpq_cmp(count[i].count, quota) >= 0)
        {
            count[i].won = true;
            winners[*winner_index].index = i;
            mpq_set(winners[*winner_index].votes, count[i].count);
            (*winner_index)++;

            eliminated[*eliminated_index] = i;
            (*eliminated_index)++;
            (*remaining_winners)--;
        }
    }
}

void redistribute_surplus(full_vote_t votes[], uint32_t total_votes, count_t count[], winner_t* winners, uint32_t winner_index, uint32_t eliminated[], uint32_t eliminated_index) {
    // loop through every vote to find votes for winners
    // may be a place to look for optimizations
    for (uint32_t i = 0; i < total_votes; i++) {
        bool win = false;
        uint32_t cand;

        // identify votes for winners
        for (uint32_t j = 0; j < winner_index; j++)
        {
            win = votes[i].cands[votes[i].cur] == winners[j].index;

            // end loop
            if (win) {
                cand = j;
                break;
            }
        }

        // ignore non-winning votes
        if (!win) continue;

        // multiply the value of a winning vote by its cand's surplus transfer value
        mpq_mul(votes[i].value, votes[i].value, winners[cand].votes);
        mpq_canonicalize(votes[i].value);

        // distribution of candidate's surplus votes
        // move to next valid cand
        votes[i].cur++;
        for (uint32_t j = 0; j < eliminated_index + 1 && votes[i].cur < votes[i].num_cands; j++) {
            if (votes[i].cands[votes[i].cur] == eliminated[j]) {
                votes[i].cur++;
                j = -1;
            }
        }

        // exhausted votes
        if (votes[i].cur >= votes[i].num_cands - 1) {
            // TODO Any ballot paper that does not express a valid preference
            // for a continuing candidate greater than the preference allocated
            // to the candidates whose surplus is to be distributed shall be set
            // aside and declared exhausted-with-value and its value added to
            // the total value of exhausted ballot papers recorded for the
            // relevant transaction in the count. Exhausted votes that form part
            // of a candidates surplus remain in the count and form part of the
            // initial candidate’s Total Vote and surplus.
        }
        else {
            // reallocate votes
            mpq_add(count[votes[i].cands[votes[i].cur]].count,
                    count[votes[i].cands[votes[i].cur]].count, votes[i].value);
            mpq_canonicalize(count[votes[i].cands[votes[i].cur]].count);
        }
    }
}

void handle_surplus(full_vote_t votes[], uint32_t total_votes, uint32_t num_cands, count_t count[], winner_t* winners, uint32_t winner_index, uint32_t eliminated[], uint32_t eliminated_index, mpq_t quota) {
    // calculation of the candidate's surplus value
    if (winner_index > 1) {
        qsort(winners, winner_index, sizeof(winner_t), cmp_winners);
    }

    // calculate surplus transfer value
    for (uint32_t i = 0; i < winner_index; i++)
    {
        // surplus value = cand's votes - quota
        mpq_sub(winners[i].votes, count[winners[i].index].count, quota);
        mpq_canonicalize(winners[i].votes);

        // surplus transfer value = surplus value / cand's votes
        mpq_div(winners[i].votes, winners[i].votes, count[winners[i].index].count);
        mpq_canonicalize(winners[i].votes);
    }

    redistribute_surplus(votes, total_votes, count, winners, winner_index, eliminated, eliminated_index);

    // allocate quota to winners
    for (uint32_t i = 0; i < num_cands; i++)
    {
        mpq_set(count[i].count, quota);
    }
}

uint32_t* count_stv(electoral_system_t vote_sys, uint32_t num_cands, full_vote_t votes[], uint64_t total_votes) {
    // int round = 1;
    uint32_t eliminated[num_cands];
    uint32_t eliminated_index = 0;
    uint32_t exclude;
    uint32_t remaining_winners = vote_sys.winners;
    mpq_t quota;
    uint32_t num_valid_votes = total_votes;
    winner_t winners[vote_sys.winners];
    uint32_t winner_index = 0;
    uint32_t* result;
    count_t count[num_cands];
    uint64_t int_count[num_cands];

    result = malloc(vote_sys.winners * sizeof(uint32_t));
    assert(result != NULL);

    if (vote_sys.winners > num_cands) {
        puts("The number of available seats must be less than the number of candidates!");
        exit(1);
    }

    while (true) {
        memset(int_count, 0, num_cands * sizeof(uint64_t));
        memset(count, 0, num_cands * sizeof(count_t));

        // initial distribution of preferences and calculation of quota
        for (uint32_t i = 0; i < total_votes; i++) {
            int_count[votes[i].cands[votes[i].cur]]++;
        }

        // convert count to fractional representation
        for (uint32_t i = 0; i < num_cands; i++) {
            mpq_init(count[i].count);
            mpq_set_ui(count[i].count, int_count[i], 1);
        }

        // initialize winner counts
        for (uint32_t i = 0; i < vote_sys.winners; i++) {
            mpq_init(winners[i].votes);
        }

        mpq_init(quota);
        // truncation is needed anyway so integer division is right
        mpq_set_ui(quota, 1 + num_valid_votes / (1 + vote_sys.winners), 1);

        if (debug) printf("quota: %f\n", mpq_get_d(quota));

        // provisional declaration of elected candidates
        check_for_winners(count, num_cands, &remaining_winners, winners, &winner_index, eliminated, &eliminated_index, quota);

        // early end
        if (remaining_winners == 0) {
            if (debug) puts("all seats filled by quota");
            break;
        }

        handle_surplus(votes, total_votes, num_cands, count, winners, winner_index, eliminated, eliminated_index, quota);

        check_for_winners(count, num_cands, &remaining_winners, winners, &winner_index, eliminated, &eliminated_index, quota);

        // TODO (optional) bulk exclusion
        exclude = find_min_count_t(count, num_cands);
        eliminated[eliminated_index++] = exclude;

        // number of seats remaining == number of candidates remaining?
        if (num_cands - eliminated_index == remaining_winners)
        {
            if (debug) puts("remaining seats == remaining candidates");
            break;
        }
    }

    for (uint32_t i = 0; i < winner_index + 1; i++) {
        result[i] = winners[i].index;
    }

    for (uint32_t i = winner_index + 1, j = 0; i < vote_sys.winners; j++)
    {
        bool present;
        for (uint32_t k = 0; k < i; k++)
        {
            present = result[k] == eliminated[k];
            if (present) break;
        }
        if (present) continue;
        result[i++] = j;
    }

    mpq_clear(quota);

    for (uint32_t i = 0; i < num_cands; i++) {
        mpq_clear(count[i].count);
    }

    for (uint32_t i = 0; i < vote_sys.winners; i++) {
        mpq_clear(winners[i].votes);
    }

    return result;
}

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
            num_valid_votes--;
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
            fprintf(output, "<tr><td>round %d votes</td>", round);
            for (uint32_t i = 0; i < num_cands; i++) {
                fprintf(output, "<td>%lu</td>", count[i]);
            }
            fputs("</tr>", output);

            if (round == 1) {
                fputs("<tr><td>original vote %</td>", output);
                for (uint32_t i = 0; i < num_cands; i++) {
                    fprintf(output, "<td>%.2f%%</td>", (double) 100 * count[i] / num_valid_votes);
                }
                fputs("</tr>", output);
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
        fputs("<tr><td>final vote %</td>", output);
        for (uint32_t i = 0; i < num_cands; i++) {
            fprintf(output, "<td>%.2f%%</td>", (double) 100 * count[i] / total_votes);
        }
        fputs("</tr>", output);
    }

    fputs("</table>", output);
    return winner;
}
