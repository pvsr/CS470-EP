#ifndef _IO_H_
#define _IO_H_

#include "votes.h"


voting_method_t parse_vote_sys(char* string);

vote_t* read_votes(char* filename, electoral_system_t* vote_sys, char*** cand_names);

#endif /* end of include guard */
