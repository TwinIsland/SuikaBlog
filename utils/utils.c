#include <stdlib.h>

#include "utils.h"

BYTE *get_sha256_encrypt(const BYTE *pass) {
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, pass, strlen((const char *)pass));

    BYTE *buf = malloc(sizeof(BYTE) * SHA256_BLOCK_SIZE);

    sha256_final(&ctx, buf);

    return buf;
}

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
