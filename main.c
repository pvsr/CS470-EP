#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <sys/stat.h>

#include "cli.h"
#include "opts.h"
#include "comms.h"
#include "votes.h"
#include "io.h"

#define CSS "table,th,td {border: 1px solid black;} table {border-collapse: collapse;} th,td {padding: 15px; text-align: center;}"

// initialize globals
bool debug = false;
bool pretty = false;
FILE* output;

int pid = -1;
int num_procs = 0;

int main(int argc, char **argv) {
    const char* method_names[] = {"FPTP", "PREFERENTIAL", "LIST", "STV"};
    char* input_file = NULL;
    char* output_file = NULL;
    char** cand_names = NULL;
    full_vote_t* votes;
    uint32_t num_cands;
    uint64_t num_votes;
    uint32_t* winners;
    int provided;
    struct stat file;
    FILE* f;
    electoral_system_t vote_sys;

    MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &provided);
    if (provided != MPI_THREAD_MULTIPLE) {
        puts("ERROR: Cannot initialize MPI in THREAD_MULTIPLE mode.");
        exit(EXIT_FAILURE);
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    assert(pid != -1);
    assert(num_procs != 0);

    vote_sys = (electoral_system_t){FPTP, 1, 3};
    assert(parse_command_line(argc, argv, &input_file, &output_file, &vote_sys) == 0);

    // only output to file on rank 0
    pretty = pretty && pid == 0;

    if (input_file == NULL) {
        puts("no votefile provided!");
        return 1;
    }

    // input is a file, not a directory
    if (stat(input_file, &file) != 0) {
        if (pid == 0) puts("input file does not exist");
        return 1;
    }

    if (!S_ISDIR(file.st_mode)) {
        if (pid != 0) {
            if (debug) printf("rank %d exiting\n", pid);
            return 0;
        }

        f = fopen(input_file, "r");
    }
    else {
        int len = strlen(input_file) + 7;
        char votefile[len];
        snprintf(votefile, len, "%s/%d.vf", input_file, pid);
        if (debug) printf("rank %d: opening %s\n", pid, votefile);
        f = fopen(votefile, "r");
    }

    if (f == NULL) {
        printf("rank %d: file does not exist\n", pid);
        return 1;
    }

    votes = read_votefile(f, &vote_sys, &cand_names, &num_cands, &num_votes);

    if (pretty) {
        if (output_file != NULL) output = fopen(output_file, "w");
        else output = stdout;
        fputs("<html><head><title>votecounter output</title><style>" CSS "</style></head><body>\n<table>\n", output);
        fputs("<tr>\n<td>candidate/party</td>\n", output);
        for (uint32_t i = 0; i < num_cands; i++) {
            if (cand_names == NULL)
                fprintf(output, "<td>%d</td>\n", i + 1);
            else
                fprintf(output, "<td>%s</td>\n", cand_names[i]);
        }
        fputs("</tr>\n", output);
    }

    if (debug && pid == 0) printf("voting system: %d-winner %s\n", vote_sys.winners, method_names[vote_sys.method]);

    winners = count_votes(vote_sys, num_cands, votes, num_votes);

    if (pid != 0) goto end;

    if (vote_sys.method == FPTP || vote_sys.method == PREFERENTIAL) {
        if (cand_names == NULL) {
            if (pretty) fprintf(output, "<p>candidate %d wins!</p>\n", winners[0] + 1);
            else printf("candidate %d wins!\n", winners[0] + 1);
        }
        else {
            if (pretty) fprintf(output, "<p>%s wins!</p>\n", cand_names[winners[0]]);
            else printf("%s wins!\n", cand_names[winners[0]]);
        }
    }
    else if (vote_sys.method == LIST) {
        for (uint32_t i = 0; i < num_cands; i++) {
            if (cand_names == NULL) {
                if (pretty) fprintf(output, "<p>party %d got %d seats!</p>\n", i + 1, winners[i]);
                else printf("party %d got %d seats!\n", i + 1, winners[i]);
            }
            else {
                if (pretty) fprintf(output, "<p>%s got %d seats!</p>\n", cand_names[i], winners[i]);
                else printf("%s got %d seats!\n", cand_names[i], winners[i]);
            }
        }
    }
    else if (vote_sys.method == STV) {
        for (uint32_t i = 0; i < vote_sys.winners; i++) {
            if (cand_names == NULL) {
                if (pretty) fprintf(output, "<p>candidate %d got a seat!</p>\n", winners[i] + 1);
                else printf("candidate %d got a seat!\n", winners[i] + 1);
            }
            else {
                if (pretty) fprintf(output, "<p>%s got a seat!</p>\n", cand_names[winners[i]]);
                else printf("%s got a seat!\n", cand_names[winners[i]]);
            }
        }
    }

    if (pretty) {
        fputs("</body></html>\n", output);
        fclose(output);
    }

end:
    if (cand_names != NULL) {
        for (uint32_t i = 0; i < num_cands; i++) free(cand_names[i]);
        free(cand_names);
    }

    for (uint32_t i = 0; i < num_votes; i++) free(votes[i].cands);
    for (uint32_t i = 0; i < num_votes; i++) mpq_clear(votes[i].value);
    free(votes);
    free(winners);

    MPI_Finalize();

    return 0;
}
