#ifndef UTILS_H
#define UTILS_H

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
                exit(1);                           \
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


#endif