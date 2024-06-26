#pragma once

#include "utils.h"

extern void exit_handler();

#define TEST(test_name, ...) void test_##test_name(##__VA_ARGS__)

#define RUN_TEST(test_name, ...) test_##test_name(##__VA_ARGS__)

#define TEST_OK(test_name, i, ...) PRINT_OK_LOG("(%s:%d) PASS", test_name, i, ##__VA_ARGS__)

#define TEST_FAILED(test_name, i, msg, ...)                               \
    do                                                                    \
    {                                                                     \
        PRINT_FAIL_LOG("(%s:%d) FAIL", msg, test_name, i, ##__VA_ARGS__); \
        if (exit_handler != NULL)                                         \
        {                                                                 \
            exit_handler();                                               \
        }                                                                 \
        exit(1);                                                          \
    } while (0)


TEST(config_test);