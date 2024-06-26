#pragma once

#include <stdio.h>

#include "string.h"

#define ERR_IS_CRITICAL 1
#define ERR_IS_IGN 0

extern void exit_handler();

// OK: info
#define PRINT_OK(info, ...)               \
    do                                    \
    {                                     \
        printf("\033[0;32m");             \
        printf("OK: ");                   \
        printf("\033[0m");                \
        printf(info "\n", ##__VA_ARGS__); \
    } while (0);

// ERROR: info
#define PRINT_ERR(info, ...)              \
    do                                    \
    {                                     \
        printf("\033[0;31m");             \
        printf("ERROR: ");                \
        printf("\033[0m");                \
        printf(info "\n", ##__VA_ARGS__); \
    } while (0);

// * info   --> ok
#define PRINT_OK_LOG(info, ...)      \
    do                               \
    {                                \
        printf("\033[0;34m");        \
        printf("* ");                \
        printf("\033[0m");           \
        printf(info, ##__VA_ARGS__); \
        printf(" \033[0;32m");       \
        printf("\t--> ok\n");        \
        printf("\033[0m");           \
    } while (0);

// * info   --> fail
#define PRINT_FAIL_LOG(info, msg, ...)   \
    do                                   \
    {                                    \
        printf("\033[0;34m");            \
        printf("* ");                    \
        printf("\033[0m");               \
        printf(info, ##__VA_ARGS__);     \
        printf(" \033[0;31m");           \
        printf("\t--> fail: %s\n", msg); \
        printf("\033[0m");               \
    } while (0);

// * info   --> ok/fail:[ret.msg]
#define PRINT_LOG(info, ret, is_err_critical, ...) \
    do                                             \
    {                                              \
        printf("\033[0;34m");                      \
        printf("* ");                              \
        printf("\033[0m");                         \
        printf(info, ##__VA_ARGS__);               \
        if (ret.status == OK)                      \
        {                                          \
            printf(" \033[0;32m");                 \
            printf("\t--> ok\n");                  \
            printf("\033[0m");                     \
        }                                          \
        else                                       \
        {                                          \
            printf("\033[0;31m");                  \
            printf("\t--> fail: %s\n", ret.msg);   \
            printf("\033[0m");                     \
            if (is_err_critical)                   \
            {                                      \
                if (exit_handler != NULL)          \
                {                                  \
                    exit_handler();                \
                }                                  \
                exit(1);                           \
            }                                      \
        }                                          \
    } while (0);

#ifdef DEBUG
#define debug(msg, ...)                             \
    do                                              \
    {                                               \
        printf("(DEBUG) " msg "\n", ##__VA_ARGS__); \
    } while (0)
#else
#define debug(msg, ...)
#endif

#define SHA256_PASS_MATCHED(pass1, pass2) strncmp((char *)pass1, (char *)pass2, SHA256_BLOCK_SIZE) == 0

// get the sha256 encrypt result from string, size equal to SHA256_BLOCK_SIZE
// The result byte list will be allocate on heap
char *get_sha256_hashed(const char *keypass);

// file system
int check_file_with_exts(const char *path, const char **exts);
int create_directory(const char *path); // create a folder in path, can be nested

// uploadfile path
#define RANDOM_UPLOAD_SUFX_LENGTH 6

char *create_upload_directory(const char *filename, const char *upload_dir);
