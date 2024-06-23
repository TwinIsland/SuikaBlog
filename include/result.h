#pragma once

enum status_t
{
    OK,
    FAILED,
};

typedef struct Result_t
{
    enum status_t status;
    const char *msg;
    void *ptr;
} Result;
