#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <mongoose.h>
#include "models.h"

#define CACHE_KEY_MAX_SIZE 32

#define ALWAYS_IN_CACHE 1
#define STANDARD_CACHE 0

typedef struct CacheEntry
{
    char key[CACHE_KEY_MAX_SIZE + 1];
    char *value;
    int always_in_flag;
    struct CacheEntry *next;
} CacheEntry;

typedef struct
{
    CacheEntry *head;
    CacheEntry *tail;
    size_t size;
} Cache;

void initialize_Cache();
char *Cache_lookup(const char *key);
void Cache_add(const char *key, char *value, int always_in_flag);
void free_Cache();

void update_cache(const char *key, const char *new_value);
void remove_cache(const char *key);

extern Cache *cache;
