#pragma once

typedef struct
{
    int is_blog_first_init; // the flag that indicate if blog just created
    int mem_usage;          // indicate the memory usage, out of 100
} SUIKA_STATE;

extern SUIKA_STATE SYSTEM_STATE;
