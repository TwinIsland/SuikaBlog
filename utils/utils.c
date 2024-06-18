#include <stdlib.h>

#include "utils.h"
#include "sha256.h"

static void bytes_to_hex(const BYTE *bytes, size_t len, char *hex_str)
{
    for (size_t i = 0; i < len; i++)
    {
        sprintf(hex_str + (i * 2), "%02x", bytes[i]);
    }
    hex_str[len * 2] = '\0';
}

char *get_sha256_hashed(const char *pass)
{
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (const BYTE *)pass, strlen(pass));

    BYTE buf[SHA256_BLOCK_SIZE];

    sha256_final(&ctx, buf);
    char *ret = malloc(SHA256_HEX_LEN+1);

    bytes_to_hex(buf, SHA256_BLOCK_SIZE, ret);

    return ret;
}

int check_file_with_exts(const char *path, const char **exts)
{
    const char **ext;

    const char *dot = strrchr(path, '.');
    if (!dot)
    {
        return 0;
    }

    for (ext = exts; *ext; ext++)
    {
        if (strcmp(dot, *ext) == 0)
        {
            return 1;
        }
    }

    return 0;
}
