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

// check if the file extend belongs to given extension list, 
// be careful that the exts list MUST END WITH NULL element
int check_file_with_exts(const char *path, const char **exts);


#endif