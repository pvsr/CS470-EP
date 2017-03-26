#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "cli.h"
#include "opts.h"
#include "votes.h"
#include "io.h"

bool debug = false;
bool pretty = false;
FILE* output;

int main(int argc, char **argv) {
    const char* method_names[] = {"FPTP", "PREFERENTIAL", "LIST", "STV"};
    char* filename = NULL;
    char** cand_names = NULL;
    full_vote_t* votes;
    int num_cands;
    int num_votes;
    int num_winners;
    int* winners;
    electoral_system_t vote_sys = (electoral_system_t){FPTP, 1};
    assert(parse_command_line(argc, argv, &filename, &vote_sys) == 0);

    if (filename != NULL) {
        votes = read_votefile(filename, &vote_sys, &cand_names, &num_cands, &num_votes);
    }
    else {
        puts("no votefile provided!");
        return 1;
    }

    if (pretty) {
        output = fopen("output.html", "w");
        fputs("<html><head><title>votecounter output</title><link rel=\"stylesheet\" type=\"text/css\" href=\"output.css\" /></head><body><table>", output);
        fputs("<tr><td>party</td>", output);
        for (int i = 0; i < num_cands; i++) {
            if (cand_names == NULL)
                fprintf(output, "<td align=\"center\">%d</td>", i + 1);
            else
                fprintf(output, "<td align=\"center\">%s</td>", cand_names[i]);
        }
        fputs("</tr>", output);
    }

    if (debug) printf("voting system: %d-winner %s\n", vote_sys.winners, method_names[vote_sys.method]);

    winners = count_votes(vote_sys, NULL, num_cands, votes, num_votes, &num_winners);

    if (vote_sys.method == FPTP) {
        if(pretty) fprintf(output, "<p>candidate %s wins!</p>", cand_names[winners[0]]);
        printf("candidate %s wins!\n", cand_names[winners[0]]);
    }
    else if (vote_sys.method == LIST) {
        for (int i = 0; i < num_winners; i++) {
            if (pretty) fprintf(output, "<p>%s got %d seats!\n</p>", cand_names[i], winners[i]);
            printf("%s got %d seats!\n", cand_names[i], winners[i]);
        }
    }

    fputs("</body></html>", output);

    return 0;
}
