#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "cli.h"
#include "debug.h"
#include "votes.h"

bool debug = false;

int main(int argc, char **argv) {
    char* filename = NULL;
    assert(parse_command_line(argc, argv, &filename) == 0);

    if (filename != NULL) {
        // read in votes from file
    }

    // temporary test data
    vote_t votes[500];
    cand_t cands[6];
    electoral_system_t sys = {FPTP, 1};

    for (int i = 0; i < 120; i++) {
        votes[i] = vote_create(0);
    }

    for (int i = 0; i < 170; i++) {
        votes[i + 120] = vote_create(1);
    }

    for (int i = 0; i < 60; i++) {
        votes[i + 290] = vote_create(2);
    }

    for (int i = 0; i < 60; i++) {
        votes[i + 350] = vote_create(3);
    }

    for (int i = 0; i < 60; i++) {
        votes[i + 410] = vote_create(4);
    }

    for (int i = 0; i < 30; i++) {
        votes[i + 470] = vote_create(5);
    }

    for (int i = 0; i < 6; i++) {
        cands[i] = (cand_t) {HOPEFUL, i};
    }

    printf("candidate %d wins!\n", count_votes(sys, cands, 6, votes, 500));

    return 0;
}
