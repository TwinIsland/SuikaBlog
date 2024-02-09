#include "fs_helper.h"

inline int file_exists(const char *filename)
{
    // Check if the file exists
    return access(filename, F_OK) == 0;
}

inline int delete_file(const char *filename)
{
    return remove(filename) == 0;
}
