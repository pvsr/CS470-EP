#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "votes.h"
#include "opts.h"
#include "listpr.h"
#include "ranked.h"

uint32_t find_max_dbl(double count[], uint32_t num_cands) {
    double max_votes = -1;
    uint32_t max_index;

    for (uint32_t i = 0; i < num_cands; i++) {
        if (count[i] > max_votes) {
            max_votes = count[i];
            max_index = i;
        }
    }

    if (debug) printf("max is %d with %f\n", max_index, max_votes);
    return max_index;
}

uint32_t find_max_int(uint64_t count[], uint32_t num_cands, uint32_t threshold) {
    uint32_t max_votes = 0;
    uint32_t max_index;

    for (uint32_t i = 0; i < num_cands; i++) {
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
uint32_t* count_fptp(int num_cands, counting_vote_t votes[], int num_votes) {
    uint64_t count[num_cands];
    memset(count, 0, num_cands * sizeof(uint64_t));
    uint32_t* winner;
    winner = malloc(sizeof(uint32_t));
    assert(winner != NULL);

    for (int i = 0; i < num_votes; i++) {
        count[votes[i].cand]++;
    }

    *winner = find_max_int(count, num_cands, num_votes / 2);

    if (pretty) {
        fputs("<tr><td>votes</td>", output);
        for (int i = 0; i < num_cands; i++) {
            fprintf(output, "<td>%lu</td>", count[i]);
        }
        fputs("</tr>", output);

        fputs("<tr><td>vote %</td>", output);
        for (int i = 0; i < num_cands; i++) {
            fprintf(output, "<td>%.2f%%</td>", (double) 100 * count[i] / num_votes);
        }
        fputs("</tr>", output);
    }

    return winner;
}

uint32_t* count_votes(electoral_system_t vote_sys, cand_t cands[] __attribute__ ((unused)), uint32_t num_cands, full_vote_t votes[], uint64_t num_votes) {
    uint32_t* result;
    counting_vote_t* cur_votes;

    cur_votes = malloc(num_votes * sizeof(counting_vote_t));
    assert(cur_votes != NULL);

    for (uint64_t i = 0; i < num_votes; i++) {
        cur_votes[i] = vote_create(votes[i]);
    }

    switch (vote_sys.method) {
        case FPTP:
            result = count_fptp(num_cands, cur_votes, num_votes);
            for (uint64_t i = 0; i < num_votes; i++)
                mpq_clear(cur_votes[i].value);
            free(cur_votes);
            return result;
        case PREFERENTIAL:
            for (uint64_t i = 0; i < num_votes; i++)
                mpq_clear(cur_votes[i].value);
            free(cur_votes);
            return count_irv(num_cands, votes, num_votes);
        case LIST:
            result = count_list(vote_sys, num_cands, cur_votes, num_votes);
            for (uint64_t i = 0; i < num_votes; i++)
                mpq_clear(cur_votes[i].value);
            free(cur_votes);
            return result;
        case STV:
            result = count_stv(vote_sys, num_cands, votes, num_votes);
            for (uint64_t i = 0; i < num_votes; i++)
                mpq_clear(cur_votes[i].value);
            free(cur_votes);
            return result;
        default:
            puts("unimplemented vote method");
            exit(1);
    }
}

counting_vote_t vote_create(full_vote_t full) {
    counting_vote_t result;
    result.cand = full.cands[0];
    mpq_init(result.value);
    // no need for canonicalization
    mpq_set_ui(result.value, 1, 1);
    return result;
}
