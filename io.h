#ifndef _IO_H_
#define _IO_H_

#include "votes.h"


voting_method_t parse_vote_sys(char* string);

full_vote_t* read_votefile(char* filename, electoral_system_t* vote_sys, char*** cand_names, int* num_cands, int* num_votes);

#endif /* end of include guard */
