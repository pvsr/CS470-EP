#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "votes.h"
#include "opts.h"

void pretty_print_results(int count[], int cand_seats[], int num_cands) {
    fputs("<tr><td>votes</td>", output);
    for (int i = 0; i < num_cands; i++) {
        fprintf(output, "<td>%d</td>", count[i]);
    }
    fputs("</tr>", output);

    fputs("<tr><td>seats</td>", output);
    for (int i = 0; i < num_cands; i++) {
        fprintf(output, "<td>%d</td>", cand_seats[i]);
    }
    fputs("</tr></table>", output);
}

// count votes in a party list election using the D'Hondt highest average method
int* count_list_high_avg(electoral_system_t vote_sys, int num_cands, counting_vote_t votes[], int num_votes, int* num_winners) {
    int winner;
    int remaining_seats = vote_sys.winners;

    double orig_count[num_cands];
    double div_count[num_cands];
    int* cand_seats;
    cand_seats = malloc(num_cands * sizeof(int));
    assert(cand_seats != NULL);
    memset(orig_count, 0, num_cands * sizeof(double)); memset(cand_seats, 0, num_cands * sizeof(int));

    for (int i = 0; i < num_votes; i++) {
        orig_count[votes[i].cand]++;
    }

    memcpy(div_count, orig_count, num_cands * sizeof(double));

    while (remaining_seats > 0) {
        winner = find_max_dbl(div_count, num_cands);

        cand_seats[winner]++;
        div_count[winner] = orig_count[winner] / (cand_seats[winner] + 1);
        remaining_seats--;
    }

    *num_winners = num_cands;
    if (pretty) {
        int int_count[num_cands];
        for (int i = 0; i < num_cands; i++) int_count[i] = orig_count[i];

        pretty_print_results(int_count, cand_seats, num_cands);
    }

    return cand_seats;
}

// count votes in a party list election using the largest remainder method
int* count_list_large_rem(electoral_system_t vote_sys, int num_cands, counting_vote_t votes[], int num_votes, int* num_winners) {
    // hare quota
    // double quota = num_votes / vote_sys.winners;
    // droop quota
    // int quota = 1 + num_votes / (1 + vote_sys.winners);
    int cur_seats;
    int winner;
    int remaining_seats = vote_sys.winners;

    int count[num_cands];
    int* cand_seats;
    cand_seats = malloc(num_cands * sizeof(int));
    assert(cand_seats != NULL);

    memset(count, 0, num_cands * sizeof(int));
    memset(cand_seats, 0, num_cands * sizeof(int));

    for (int i = 0; i < num_votes; i++) {
        count[votes[i].cand]++;
    }

    for (int i = 0; i < num_cands; i++) {
        // TODO this method isn't working properly. why?
        // cur_seats = count[i] / quota;
        cur_seats = count[i] * vote_sys.winners / num_votes;
        if (debug) printf("party %d gets %d seats outright\n", i, cur_seats);
        cand_seats[i] += cur_seats;
        remaining_seats -= cur_seats;

        // count = count[i] % quota;
        count[i] = count[i] * vote_sys.winners % num_votes;
    }

    while (remaining_seats > 0) {
        winner = find_max_int(count, num_cands, INT_MAX);
        if (debug) printf("party %d gets a remainder seat\n", winner);
        cand_seats[winner]++;
        count[winner] = 0;
        remaining_seats--;
    }
    
    *num_winners = num_cands;
    if (pretty) pretty_print_results(count, cand_seats, num_cands);

    return cand_seats;
}

// use whichever method for now
int* count_list(electoral_system_t vote_sys, int num_cands, counting_vote_t votes[], int num_votes, int* num_winners) {
    return count_list_high_avg(vote_sys, num_cands, votes, num_votes, num_winners);
}
