#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "opts.h"

#define MAX_LINE 40
#define MAX_LINE_TERM 39

// from stackoverflow.com/questions/25410690/scanf-variable-length-specifier
#define EXPAND2(x) #x
#define EXPAND(X) EXPAND2(X)

voting_method_t parse_vote_sys(char* string) {
    if (strcmp("fptp", string) == 0) {
        return FPTP;
    }
    else if (strcmp("list", string) == 0) {
        return LIST;
    }
    else {
        if (debug) printf("'%s' is an invalid counting method, using fptp\n", string);
        return FPTP;
    }
}

full_vote_t* parse_votes_count(FILE* f, char* str, int num_cands, int* num_votes) {
    char c = '\0';
    int i;
    uint64_t cur_vote;
    int vote_size;
    fpos_t start_pos;
    fpos_t end_pos;
    full_vote_t* result;
    full_vote_t vote;

    cur_vote = 0;

    // next line
    fgetc(f);
    assert(fgetpos(f, &start_pos) == 0);

    while (true) {
        if (fscanf(f, "%d %*s ", &i) != 1) {
            if (fgetc(f) == '!') break;
            else {
                puts("invalid vote count format");
                exit(1);
            }
        }

        *num_votes += i;
    }

    assert(fgetpos(f, &end_pos) == 0);
    assert(fsetpos(f, &start_pos) == 0);

    result = malloc(*num_votes * sizeof(full_vote_t));
    assert(result != NULL);

    i = 0;
    while (true) {
        if (fscanf(f, "%d %s ", &i, str) != 2) {
            if (fgetc(f) == '!') break;
            else {
                puts("invalid vote count format");
                exit(1);
            }
        }

        vote_size = 1;
        for (unsigned int j = 0; j < strlen(str); j++) {
            if (str[j] == ',') vote_size++;
        }

        vote_size = vote_size > num_cands ? num_cands : vote_size;

        vote.cands = malloc(vote_size * sizeof(int));
        assert(vote.cands != NULL);
        vote.num_cands = vote_size;

        int n = 0;
        for (unsigned int j = 0; j < strlen(str) && n < vote_size; j++) {
            c = str[j];
            if (c != ',') vote.cands[n++] = c - 48;
        }

        for (int j = 0; j < i; j++, cur_vote++) {
            result[cur_vote].cands = malloc(vote_size * sizeof(int));
            assert(result[cur_vote].cands != NULL);

            for (int k = 0; k < vote.num_cands; k++) 
                result[cur_vote].cands[k] = vote.cands[k];
            result[cur_vote].num_cands = vote.num_cands;
            assert(result[cur_vote].num_cands > 0);
        }

        free(vote.cands);
    }

    assert(fsetpos(f, &end_pos) == 0);

    fclose(f);
    return result;
}

full_vote_t* parse_votes_list(FILE* f, char* str, int num_cands, int* num_votes);

int parse_candidates(FILE* f, char* str, char*** cand_names) {
    char c;
    char** names;
    int num_cands = 0;
    fpos_t start_pos;
    fpos_t end_pos;

    // next line
    fgetc(f);
    assert(fgetpos(f, &start_pos) == 0);

    while (true) {
        c = fgetc(f);
        if (c == '!') break;
        else if (c == '\n') num_cands++;
    }

    assert(fgetpos(f, &end_pos) == 0);
    assert(fsetpos(f, &start_pos) == 0);

    if (num_cands <= 0) {
        if (debug) puts("empty cand names block!");
        return -1;
    }

    names = malloc(num_cands * sizeof(char*));
    assert(names != NULL);

    for (int i = 0; i < num_cands; i++) {
        // TODO if a name is >MAX_LINE chars, weird things will happen
        fgets(str, MAX_LINE, f);
        strtok(str, "\n");
        names[i] = malloc(strlen(str) + 1);
        strncpy(names[i], str, strlen(str) + 1);
        assert(names[i] != NULL);
    }

    assert(fsetpos(f, &end_pos) == 0);

    *cand_names = names;
    return num_cands;
}

full_vote_t* read_votefile(char* filename, electoral_system_t* vote_sys, char*** cand_names, int* num_cands, int* num_votes) {
    char str[MAX_LINE];
    char c;
    int i;
    FILE* f;

    *num_cands = 0;
    *num_votes = 0;

    f = fopen(filename, "r");

    if (f == NULL) {
        puts("nonexistent votefile");
        exit(1);
    }

    while (!feof(f)) {
        c = fgetc(f);

        switch (c) {
            case '-':
                // TODO better way to do this with fscanf
                while (fgetc(f) != '\n');
                break;
            case 'm':
                fgetc(f);
                fscanf(f, "%" EXPAND(MAX_LINE_TERM) "s", str);
                vote_sys->method = parse_vote_sys(str);
                break;
            case 's':
                fgetc(f);
                fscanf(f, "%d", &i);
                vote_sys->winners = i;
                break;
            case 'p':
                *num_cands = parse_candidates(f, str, cand_names);
                break;
            case 'n':
                fgetc(f);
                fscanf(f, "%d", num_cands);
                break;
            case 'c':
                return parse_votes_count(f, str, *num_cands, num_votes);
            case 'v':
                // return parse_votes_list(f, str, *num_cands, num_votes);
            case '\n':
            case ' ':
            default:
                break;
        }
    }

    fclose(f);
    return NULL;
}
