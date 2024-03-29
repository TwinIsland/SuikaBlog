#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#include "sha256.h"
#include "string.h"

#define ERR_IS_CRITICAL 1
#define ERR_IS_IGN 0

// OK: info
#define PRINT_OK(info, ...)               \
    do                                    \
    {                                     \
        printf("\033[0;32m");            \
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

// * info   --> ok/fail:[ret.msg]
#define PRINT_LOG(info, ret, is_err_critical, exit_handler, ...) \
    do                                                           \
    {                                                            \
        printf("\033[0;34m");                                    \
        printf("* ");                                            \
        printf("\033[0m");                                       \
        printf(info, ##__VA_ARGS__);                             \
        if (ret.status == OK)                                    \
        {                                                        \
            printf(" \033[0;32m");                               \
            printf("\t--> ok\n");                                \
            printf("\033[0m");                                   \
        }                                                        \
        else                                                     \
        {                                                        \
            printf("\033[0;31m");                                \
            printf("\t--> fail: %s\n", ret.msg);                 \
            printf("\033[0m");                                   \
            if (is_err_critical)                                 \
            {                                                    \
                if (exit_handler != NULL)                        \
                {                                                \
                    exit_handler();                              \
                }                                                \
                exit(1);                                         \
            }                                                    \
        }                                                        \
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
BYTE *get_sha256_encrypt(const BYTE *keypass);

#endif