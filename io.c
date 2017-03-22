#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "debug.h"

#define MAX_LINE 20

voting_method_t parse_vote_sys(char* string) {
    if (strcmp("fptp", string) == 0) {
        return FPTP;
    }
    else if (strcmp("list", string) == 0) {
        return LIST;
    }
    else {
        if (debug) puts("invalid counting method, using fptp");
        return FPTP;
    }
}

int parse_candidates(FILE* f, char* str, char*** cand_names) {
    char c;
    cand_t* cands;
    int num_cands = 0;
    fpos_t start_pos;
    fpos_t end_pos;

    // next line
    fgetc(f);
    assert(fgetpos(f, &start_pos) == 0);

    while (fgetc(f) != '!') {
        if (fgetc(f) == '\n') num_cands++;
    }

    assert(fgetpos(f, &end_pos) == 0);
    assert(fsetpos(f, &start_pos) == 0);

    if (num_cands <= 0) {
        if (debug) puts("empty cand names block!");
        return -1;
    }

    *cand_names = malloc(num_cands * sizeof(char*));
    assert(cands != NULL);

    for (int i = 0; i < num_cands; i++) {
        // TODO if a name is >MAX_LINE chars, weird things will happen
        fgets(str, MAX_LINE, f);
        cand_names[i] = malloc(strlen(str) + 1);
        assert(cand_names[i] != NULL);
    }

    assert(fsetpos(f, &end_pos) == 0);

    return num_cands;
}

vote_t* read_votes(char* filename, electoral_system_t* vote_sys, char*** cand_names) {
    char str[MAX_LINE];
    char c;
    int i;
    int num_cands = 0;
    FILE* f;
    f = fopen(filename, "r");

    if (f == NULL) {
        puts("nonexistent file");
        return NULL;
    }

    while (!feof(f)) {
        c = fgetc(f);

        switch (c) {
            case '-':
                // TODO better way to do this with fscanf
                while (fgetc(f) != '\n');
                break;
            case '\n':
            case ' ':
                continue;
            case 'm':
                fgetc(f);
                fgets(str, MAX_LINE, f);
                vote_sys->method = parse_vote_sys(str);
                break;
            case 's':
                fgetc(f);
                fscanf(f, "%d", i);
                vote_sys->winners = i;
                break;
            case 'p':
                num_cands = parse_candidates(f, str, cand_names);
                break;
            case 'n':
                fgetc(f);
                fscanf(f, "%d", num_cands);
                break;
            default:
                break;
        }
    }
}
