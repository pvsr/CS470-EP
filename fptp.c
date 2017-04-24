#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <mpi.h>

#include "fptp.h"
#include "opts.h"
#include "comms.h"

uint32_t find_max_int(uint64_t count[], uint32_t num_cands, uint32_t threshold) {
    uint32_t max_votes = 0;
    uint32_t max_index;

    for (uint32_t i = 0; i < num_cands; i++) {
        if (count[i] > max_votes) {
            max_votes = count[i];
            max_index = i;
        }

        if (count[i] > threshold) {
            goto end;
        }
    }

end:
    if (debug) printf("max is %d with %d\n", max_index, max_votes);
    return max_index;
}

// count votes in an fptp election
uint32_t* count_fptp(uint32_t num_cands, uint32_t votes[], uint64_t num_votes) {
    uint64_t count[num_cands + 1];
    memset(count, 0, num_cands * sizeof(uint64_t));
    uint32_t* winner;
    winner = malloc(sizeof(uint32_t));
    assert(winner != NULL);

    for (uint64_t i = 0; i < num_votes; i++) {
        count[votes[i]]++;
    }

    // put num_votes in with count so we can sum it
    count[num_cands] = num_votes;

    if (num_procs > 1) {
        if (pid == 0)
        {
            MPI_Reduce(MPI_IN_PLACE, count, num_cands + 1, MPI_UNSIGNED_LONG,
                       MPI_SUM, 0, MPI_COMM_WORLD);
            num_votes = count[num_cands];
        }
        else
        {
            MPI_Reduce(count, NULL, num_cands + 1, MPI_UNSIGNED_LONG,
                       MPI_SUM, 0, MPI_COMM_WORLD);
            // the rest is cleanup
            return NULL;
        }
    }

    *winner = find_max_int(count, num_cands, num_votes / 2);

    if (pretty) {
        fputs("<tr>\n<td>votes</td>\n", output);
        for (uint64_t i = 0; i < num_cands; i++) {
            fprintf(output, "<td>%lu</td>\n", count[i]);
        }
        fputs("</tr>\n", output);

        fputs("<tr>\n<td>vote %</td>\n", output);
        for (uint64_t i = 0; i < num_cands; i++) {
            fprintf(output, "<td>%.2f%%</td>\n", (double) 100 * count[i] / num_votes);
        }
        fputs("</tr>\n</table>\n", output);
    }

    return winner;
}
