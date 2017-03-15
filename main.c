#include "cli.h"

int main(int argc, char **argv) {
    char* filename;
    return parse_command_line(argc, argv, &filename);
}
