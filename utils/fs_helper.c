#include <string.h>
#include "fs_helper.h"

int is_file_image(const char *path) {
    const char *image_extensions[] = {".png", ".jpg", ".jpeg", ".webp", ".gif", ".svg", NULL};
    const char **ext;

    const char *dot = strrchr(path, '.');
    if (!dot) {
        return 0; 
    }

    for (ext = image_extensions; *ext; ext++) {
        if (strcmp(dot, *ext) == 0) {
            return 1; // Match found, it's an image
        }
    }

    return 0; // No match found in the list of image extensions
}
