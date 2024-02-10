#include "ipc.h"

static int ipc_fd = -1;
static char *ipc_memory = NULL;

static char *IPC_FILE_PATH;
static int IPC_SIZE;

int init_ipc(configuration *config)
{

    IPC_FILE_PATH = config->ipc_path;
    IPC_SIZE = config->ipc_size;

    ipc_fd = open(IPC_FILE_PATH, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    if (ipc_fd == -1)
    {
        perror("Error opening file for IPC");
        return -1;
    }

    if (lseek(ipc_fd, IPC_SIZE - 1, SEEK_SET) == -1)
    {
        close(ipc_fd);
        perror("Error calling lseek() to 'stretch' the file");
        return -1;
    }

    if (write(ipc_fd, "", 1) == -1)
    {
        close(ipc_fd);
        perror("Error writing last byte of the file");
        return -1;
    }

    ipc_memory = mmap(0, IPC_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, ipc_fd, 0);
    if (ipc_memory == MAP_FAILED)
    {
        close(ipc_fd);
        perror("Error mmapping the file");
        return -1;
    }

    return 0;
}

void send_ipc(const char *message)
{
    if (ipc_memory != NULL)
    {
        ipc_memory[0] = IPC_INCOMPLETE;
        size_t message_length = strlen(message);
        if (message_length > IPC_SIZE - 2)
            message_length = IPC_SIZE - 2;

        strncpy(ipc_memory + 1, message, message_length);
        ipc_memory[1 + message_length] = '\0';
        ipc_memory[0] = IPC_SENT;
        printf("sended!\n");
    }
    else {
        fprintf(stderr, "IPC not initialized or mmap failed.\n");
    }
        
}

void read_ipc()
{
    if (ipc_memory != NULL)
    {
        if (ipc_memory[0] == IPC_SENT)
        {
            printf("%s\n", ipc_memory + 1);
            ipc_memory[0] = IPC_READ;
        }
        else if (ipc_memory[0] == IPC_INCOMPLETE || ipc_memory[0] == IPC_READ)
            fprintf(stderr, "No new message or message already read.\n");
    }
    else
        fprintf(stderr, "IPC not initialized or mmap failed.\n");
}

void reset_ipc()
{
    if (ipc_memory != NULL)
        memset(ipc_memory, 0, IPC_SIZE);
    else
        fprintf(stderr, "IPC not initialized or mmap failed.\n");
}

void cleanup_ipc()
{
    if (ipc_memory != NULL)
    {
        munmap(ipc_memory, IPC_SIZE);
        ipc_memory = NULL;
    }
    if (ipc_fd != -1)
    {
        close(ipc_fd);
        ipc_fd = -1;
    }
    unlink(IPC_FILE_PATH);
}
