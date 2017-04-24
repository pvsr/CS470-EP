#ifndef _COMMS_H_
#define _COMMS_H_

#include <stdbool.h>
#include <stdio.h>

extern int pid;
extern int num_procs;

void* server_task(void* args);

#endif /* end of include guard */
