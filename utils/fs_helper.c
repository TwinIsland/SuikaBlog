#include <string.h>
#include "fs_helper.h"

int check_file_with_exts(const char *path, const char **exts) {
    const char **ext;

    const char *dot = strrchr(path, '.');
    if (!dot) {
        return 0; 
    }

    for (ext = exts; *ext; ext++) {
        if (strcmp(dot, *ext) == 0) {
            return 1; 
        }
    }

    return 0; 
}
