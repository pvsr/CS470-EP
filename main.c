#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "cli.h"
#include "debug.h"
#include "votes.h"
#include "io.h"

bool debug = false;

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

    if (debug) printf("voting system: %d-winner %s\n", vote_sys.winners, method_names[vote_sys.method]);

    count_votes(vote_sys, NULL, num_cands, votes, num_votes);

    return 0;
}
