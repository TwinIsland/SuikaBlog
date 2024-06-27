#include <stdlib.h>
#include <time.h>

#include "utils.h"
#include "sha256.h"
#include "mongoose.h"
#include "config.h"

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
    char *ret = malloc(SHA256_HEX_LEN + 1);

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

int create_directory(const char *path)
{
    char tmp[256];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;

    for (p = tmp + 1; *p; p++)
    {
        if (*p == '/')
        {
            *p = 0;
            if (mkdir(tmp, S_IRWXU) != 0 && errno != EEXIST)
            {
                return -1;
            }
            *p = '/';
        }
    }

    if (mkdir(tmp, S_IRWXU) != 0 && errno != EEXIST)
    {
        return -1;
    }

    return 0;
}

char *create_upload_directory(const char *filename, const char *upload_dir)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    size_t dir_len = strlen(upload_dir) + strlen(filename) + RANDOM_UPLOAD_SUFX_LENGTH + 16;

    char pwd_buf[RANDOM_UPLOAD_SUFX_LENGTH + 1];
    mg_random_str(pwd_buf, RANDOM_UPLOAD_SUFX_LENGTH + 1);

    char *upload_path = malloc(dir_len);
    snprintf(upload_path, dir_len, "%s/%d/%02d", upload_dir, tm.tm_year + 1900, tm.tm_mon + 1);

    if (create_directory(upload_path) != 0)
    {
        free(upload_path);
        return NULL;
    }

    snprintf(upload_path, dir_len, "%s/%d/%02d/%s_%s", upload_dir, tm.tm_year + 1900, tm.tm_mon + 1, pwd_buf, filename);

    return upload_path;
}
