#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>

#include "cli.h"

void print_help_message() {
    puts("Usage: votecounter [OPTION]...\n"
         "Options:\n"
         "  -h --help           display this help message\n"
         "  -f --votefile=FILE  read votes from file");
}

int parse_command_line(int argc, char **argv, char **filename) {
    int opt;

    if (argv == NULL) return 1;

    while (true) {
        static struct option long_options[] = {
            {"help", no_argument, 0, 'h'},
            {"votefile", required_argument, 0, 'f'},
            {0, 0, 0, 0}
        };

        int option_index = 0;

        opt = getopt_long(argc, argv, "hf", long_options, &option_index);

        if (opt == -1) break;

        switch (opt) {
            case 'h':
                print_help_message();
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
