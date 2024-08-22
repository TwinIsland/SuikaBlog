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
    void *value;
    int always_in_flag;
    void (*destroyer)(void *value); // default use free
    struct CacheEntry *next;
} CacheEntry;

typedef struct
{
    CacheEntry *head;
    CacheEntry *tail;
    size_t size;
} Cache;

#define STR_CACHE_DESTROYER free

void initialize_Cache();
void *Cache_lookup(const char *key);
void Cache_add(const char *key, void *value, int always_in_flag, void (*destroyer)(void *value));
void free_Cache();

void update_cache(const char *key, void *new_value);
void remove_cache(const char *key);

extern Cache *cache;
