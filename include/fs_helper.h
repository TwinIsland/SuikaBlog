#ifndef FS_HELPER_H
#define FS_HELPER_H

#include <stdio.h>
#include <unistd.h> // For access()

static inline int file_exists(const char *filename)
{
    return access(filename, F_OK) == 0;
}

static inline int delete_file(const char *filename)
{
    return remove(filename) == 0;
}

int is_file_image(const char *path);


#endif