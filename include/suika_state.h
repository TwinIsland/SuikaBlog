#pragma once

typedef struct
{
    int is_db_first_initialize; // the flag that indicate if the database just created
    int mem_usage;              // indicate the memory usage, out of 100
} SUIKA_STATE;

extern SUIKA_STATE SYSTEM_STATE;
