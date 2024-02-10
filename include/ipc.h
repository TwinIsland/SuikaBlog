#ifndef IPC_LIB_H
#define IPC_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>

#include "ini_handler.h"

typedef enum ipc_status_t
{
    IPC_INCOMPLETE = '0',
    IPC_SENT = '1',
    IPC_READ = '2',
} ipc_status;

int init_ipc(configuration *config);
void send_ipc(const char *message);
void read_ipc(void);
void reset_ipc(void);
void cleanup_ipc(void);

#endif // IPC_LIB_H
