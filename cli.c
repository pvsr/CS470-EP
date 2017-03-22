#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"
#include "debug.h"

void print_help_message() {
    puts("Usage: votecounter [OPTION]...\n"
         "Options:\n"
         "  -h --help               display this help message\n"
         "  -v --votesystem=SYSTEM  voting system\n"
         "  -s --seats=SEATS        number of seats in multiwinner systems\n"
         "  -d --debug              show debug output\n"
         "  -f --votefile=FILE      read votes from file");
}

void parse_vote_sys(char* string, electoral_system_t* vote_sys) {
    if (strcmp("fptp", string) == 0) {
        vote_sys->method = FPTP;
    }
    else if (strcmp("list", string) == 0) {
        vote_sys->method = LIST;
    }
    else {
        puts("invalid counting method");
        exit(1);
    }
}

int parse_command_line(int argc, char** argv, char** filename, electoral_system_t* vote_sys) {
    int opt;

    if (argv == NULL) return 1;

    while (true) {
        static struct option long_options[] = {
            {"help", no_argument, 0, 'h'},
            {"votesystem", required_argument, 0, 'v'},
            {"seats", required_argument, 0, 's'},
            {"debug", no_argument, 0, 'd'},
            {"votefile", required_argument, 0, 'f'},
            {0, 0, 0, 0}
        };

        int option_index = 0;

        opt = getopt_long(argc, argv, "hvsdf", long_options, &option_index);

        if (opt == -1) break;

        switch (opt) {
            case 'h':
                print_help_message();
                break;

            case 'v':
                parse_vote_sys(argv[optind++], vote_sys);
                break;

            case 's':
                vote_sys->winners = atoi(argv[optind++]);
                break;

            case 'd':
                debug = true;
                break;

            case 'f':
                // set file to next arg
                if (optind < argc) *filename = argv[optind++];
                // no file
                else print_help_message();
                break;

            default:
                print_help_message();
                return 1;
        }
    }

    return 0;
}
