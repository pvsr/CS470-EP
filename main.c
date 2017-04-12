#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "cli.h"
#include "opts.h"
#include "votes.h"
#include "io.h"

#define CSS "table,th,td {border: 1px solid black;} table {border-collapse: collapse;} th,td {padding: 15px; text-align: center;}"

bool debug = false;
bool pretty = false;
FILE* output;

int main(int argc, char **argv) {
    const char* method_names[] = {"FPTP", "PREFERENTIAL", "LIST", "STV"};
    char* votefile = NULL;
    char* output_file = NULL;
    char** cand_names = NULL;
    full_vote_t* votes;
    uint32_t num_cands;
    uint64_t num_votes;
    uint32_t* winners;
    electoral_system_t vote_sys = (electoral_system_t){FPTP, 1, 3};
    assert(parse_command_line(argc, argv, &votefile, &output_file, &vote_sys) == 0);

    if (votefile != NULL) {
        votes = read_votefile(votefile, &vote_sys, &cand_names, &num_cands, &num_votes);
    }
    else {
        puts("no votefile provided!");
        return 1;
    }

    if (pretty) {
        if (output_file != NULL) output = fopen(output_file, "w");
        else output = stdout;
        fputs("<html><head><title>votecounter output</title><style>" CSS "</style></head><body><table>", output);
        fputs("<tr><td>candidate/party</td>", output);
        for (uint32_t i = 0; i < num_cands; i++) {
            if (cand_names == NULL)
                fprintf(output, "<td>%d</td>", i + 1);
            else
                fprintf(output, "<td>%s</td>", cand_names[i]);
        }
        fputs("</tr>", output);
    }

    if (debug) printf("voting system: %d-winner %s\n", vote_sys.winners, method_names[vote_sys.method]);

    winners = count_votes(vote_sys, NULL, num_cands, votes, num_votes);

    if (vote_sys.method == FPTP || vote_sys.method == PREFERENTIAL) {
        if (cand_names == NULL) {
            if (pretty) fprintf(output, "<p>candidate %d wins!</p>", winners[0] + 1);
            else printf("candidate %d wins!\n", winners[0] + 1);
        }
        else {
            if (pretty) fprintf(output, "<p>%s wins!</p>", cand_names[winners[0]]);
            else printf("%s wins!\n", cand_names[winners[0]]);
        }
    }
    else if (vote_sys.method == LIST) {
        for (uint32_t i = 0; i < num_cands; i++) {
            if (cand_names == NULL) {
                if (pretty) fprintf(output, "<p>party %d got %d seats!</p>", i + 1, winners[i]);
                else printf("party %d got %d seats!\n", i + 1, winners[i]);
            }
            else {
                if (pretty) fprintf(output, "<p>%s got %d seats!</p>", cand_names[i], winners[i]);
                else printf("%s got %d seats!\n", cand_names[i], winners[i]);
            }
        }
    }

    if (pretty) {
        fputs("</body></html>\n", output);
        fclose(output);
    }

    if (cand_names != NULL) {
        for (uint32_t i = 0; i < num_cands; i++) free(cand_names[i]);
        free(cand_names);
    }

    for (uint32_t i = 0; i < num_votes; i++) free(votes[i].cands);
    free(votes);
    free(winners);

    return 0;
}
