#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "votes.h"
#include "opts.h"

void pretty_print_results(uint64_t count[], uint32_t cand_seats[], uint32_t num_cands, uint64_t num_votes, uint32_t num_seats) {
    fputs("<tr>\n<td>votes</td>\n", output);
    for (uint32_t i = 0; i < num_cands; i++) {
        fprintf(output, "<td>%lu</td>\n", count[i]);
    }
    fputs("</tr>\n", output);

    fputs("<tr>\n<td>vote %</td>\n", output);
    for (uint32_t i = 0; i < num_cands; i++) {
        fprintf(output, "<td>%.2f%%</td>\n", (double) 100 * count[i] / num_votes);
    }
    fputs("</tr>\n", output);

    fputs("<tr>\n<td>seats</td>\n", output);
    for (uint32_t i = 0; i < num_cands; i++) {
        fprintf(output, "<td>%d</td>\n", cand_seats[i]);
    }
    fputs("</tr>\n", output);

    fputs("<tr>\n<td>seat %</td>\n", output);
    for (uint32_t i = 0; i < num_cands; i++) {
        fprintf(output, "<td>%.2f%%</td>\n", (double) 100 * cand_seats[i] / num_seats);
    }
    fputs("</tr>\n</table>\n", output);
}

// count votes in a party list election using the D'Hondt highest average method
uint32_t* count_list_high_avg(electoral_system_t vote_sys, uint32_t num_cands, min_vote_t votes[], uint64_t num_votes) {
    uint32_t winner;
    uint32_t remaining_seats = vote_sys.winners;

    double orig_count[num_cands];
    double div_count[num_cands];
    uint32_t* cand_seats;
    cand_seats = malloc(num_cands * sizeof(uint32_t));
    assert(cand_seats != NULL);
    memset(orig_count, 0, num_cands * sizeof(double));
    memset(cand_seats, 0, num_cands * sizeof(uint32_t));

    for (uint32_t i = 0; i < num_votes; i++) {
        orig_count[votes[i]]++;
    }

    memcpy(div_count, orig_count, num_cands * sizeof(double));

    for (uint32_t i = 0; i < num_cands; i++) {
        if (100 * orig_count[i] / num_votes < vote_sys.threshold) div_count[i] = 0;
    }

    while (remaining_seats > 0) {
        winner = find_max_dbl(div_count, num_cands);

        cand_seats[winner]++;
        div_count[winner] = orig_count[winner] / (cand_seats[winner] + 1);
        remaining_seats--;
    }

    if (pretty) {
        uint64_t int_count[num_cands];
        for (uint32_t i = 0; i < num_cands; i++) int_count[i] = orig_count[i];

        pretty_print_results(int_count, cand_seats, num_cands, num_votes, vote_sys.winners);
    }

    return cand_seats;
}

// count votes in a party list election using the largest remainder method
uint32_t* count_list_large_rem(electoral_system_t vote_sys, uint32_t num_cands, min_vote_t votes[], uint64_t num_votes) {
    // hare quota
    // double quota = num_votes / vote_sys.winners;
    // droop quota
    // uint32_t quota = 1 + num_votes / (1 + vote_sys.winners);
    uint32_t cur_seats;
    uint32_t winner;
    uint32_t remaining_seats = vote_sys.winners;
    uint32_t num_votes_adjusted = num_votes;

    bool failed_threshold[num_cands];
    uint64_t count[num_cands];
    uint32_t* cand_seats;
    cand_seats = malloc(num_cands * sizeof(uint32_t));
    assert(cand_seats != NULL);

    memset(count, 0, num_cands * sizeof(uint64_t));
    memset(cand_seats, 0, num_cands * sizeof(uint32_t));

    for (uint32_t i = 0; i < num_votes; i++) {
        count[votes[i]]++;
    }

    for (uint32_t i = 0; i < num_cands; i++) {
        failed_threshold[i] = count[i] / num_votes < vote_sys.threshold;
        if (failed_threshold[i]) num_votes_adjusted -= count[i];
    }

    for (uint32_t i = 0; i < num_cands; i++) {
        if (failed_threshold[i]) continue; 

        // TODO this method isn't working properly. why?
        // cur_seats = count[i] / quota;
        cur_seats = count[i] * vote_sys.winners / num_votes_adjusted;
        if (debug) printf("party %d gets %d seats outright\n", i, cur_seats);
        cand_seats[i] += cur_seats;
        remaining_seats -= cur_seats;

        // count = count[i] % quota;
        count[i] = count[i] * vote_sys.winners % num_votes_adjusted;
    }

    while (remaining_seats > 0) {
        winner = find_max_int(count, num_cands, INT_MAX);
        if (debug) printf("party %d gets a remainder seat\n", winner);

        if (!failed_threshold[winner]) {
            cand_seats[winner]++;
            remaining_seats--;
        }
        count[winner] = 0;
    }
    
    if (pretty) pretty_print_results(count, cand_seats, num_cands, num_votes, vote_sys.winners);

    return cand_seats;
}

// use whichever method for now
uint32_t* count_list(electoral_system_t vote_sys, uint32_t num_cands, min_vote_t votes[], uint64_t num_votes) {
    return count_list_high_avg(vote_sys, num_cands, votes, num_votes);
}
