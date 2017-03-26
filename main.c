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
    char** cand_names;
    full_vote_t* votes;
    int num_cands;
    int num_votes;
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
    }

    if (debug) printf("voting system: %d-winner %s\n", vote_sys.winners, method_names[vote_sys.method]);

    count_votes(vote_sys, NULL, num_cands, votes, num_votes);

    fputs("</body></html>", output);

    return 0;
}
