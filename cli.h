#ifndef _CLI_H_
#define _CLI_H_

#include "votes.h"

int parse_command_line(int argc, char **argv, char **vote_file, char** output_file, electoral_system_t* vote_sys);

#endif /* end of include guard */
