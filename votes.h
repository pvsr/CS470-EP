#ifndef _VOTES_H_
#define _VOTES_H_

typedef struct electoral_system {
    voting_method method;
    int winners;
} electoral_system_t;

enum voting_method {
    // plurality systems
    FPTP, // first past the post
    PREFERENTIAL, // instant runoff
    // proportional systems
    LIST, // party list
    STV // single transferable vote
};

// read votes from a file
// TODO vote file format
void read_votes(char* filename);

#endif /* end of include guard */
