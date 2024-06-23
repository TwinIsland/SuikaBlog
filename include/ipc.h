#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>

typedef enum ipc_status_t
{
    IPC_INCOMPLETE = '0',
    IPC_SENT = '1',
    IPC_READ = '2',
} ipc_status;

int init_ipc();
void send_ipc(const char *message);
void read_ipc(void);
void reset_ipc(void);
void cleanup_ipc(void);
