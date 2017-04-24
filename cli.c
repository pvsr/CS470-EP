#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"
#include "opts.h"
#include "io.h"

void print_help_message() {
    puts("Usage: votecounter [OPTION]... VOTEFILE\n"
         "Options:\n"
         "  -h --help               display this help message\n"
         "  -T --timing             print timing information\n"
         "  -m --votemethod=METHOD  voting system (fptp, list, etc.)\n"
         "  -s --seats=NUM          number of seats in multiwinner systems\n"
         "  -t --threshold=NUM      percent threshold to win a seat in list-pr (default 3)\n"
         "  -d --debug              show debug output\n"
         "  -p --pretty             generate pretty HTML output\n"
         "  -o --output=FILE        output location for -p");
}

int parse_command_line(int argc, char** argv, char** vote_file, char** output_file, electoral_system_t* vote_sys) {
    int opt;
    int i;

    if (argv == NULL) return 1;

    while (true) {
        static struct option long_options[] = {
            {"help", no_argument, 0, 'h'},
            {"timing", no_argument, 0, 'T'},
            {"votemethod", required_argument, 0, 'v'},
            {"seats", required_argument, 0, 's'},
            {"threshold", required_argument, 0, 't'},
            {"debug", no_argument, 0, 'd'},
            {"pretty", no_argument, 0, 'p'},
            {"output", required_argument, 0, 'o'},
            {0, 0, 0, 0}
        };

        int option_index = 0;

        opt = getopt_long(argc, argv, "hTm:s:t:dpo:", long_options, &option_index);

        if (opt == -1) break;

        switch (opt) {
            case 'm':
                vote_sys->method = parse_vote_sys(optarg);
                break;
            case 's':
                i = atoi(optarg);
                if (i <= 0) {
                    puts("invalid number of winners");
                    exit(1);
                }
                vote_sys->winners = atoi(optarg);
                break;
            case 't':
                i = atoi(optarg);
                if (i > 100 || i < 0) {
                    puts("invalid threshold");
                    exit(1);
                }
                vote_sys->threshold = i;
                break;
            case 'd':
                debug = true;
                break;
            case 'T':
                timing = true;
                break;
            case 'p':
                pretty = true;
                break;
            case 'o':
                *output_file = optarg;
                break;
            case 'h':
            default:
                print_help_message();
                exit(0);
        }
    }

    *vote_file = argv[optind++];

    return 0;
}
