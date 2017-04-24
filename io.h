#ifndef _IO_H_
#define _IO_H_

#include "votes.h"


voting_method_t parse_vote_sys(char* string);

full_vote_t* read_votefile(FILE* f, electoral_system_t* vote_sys, char*** cand_names, uint32_t* num_cands, uint64_t* num_votes);

#endif /* end of include guard */
