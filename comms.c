#include <pthread.h>

#include "comms.h"

void* server_task(void* args) {
    pthread_exit(args);
}
