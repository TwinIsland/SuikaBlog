#ifndef CACHE_H
#define CACHE_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <mongoose.h>
#include "config_loader.h"
#include "models.h"

#define CACHE_KEY_MAX_SIZE 32

typedef struct CacheEntry
{
    char key[CACHE_KEY_MAX_SIZE+1]; 
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

Cache *initialize_Cache();
char *Cache_lookup(Cache *cache, const char *key);
void Cache_add(Cache *cache, const char *key, char *value, int always_in_flag);
void free_Cache(Cache *cache);

void update_cache(Cache *cache, const char *key, const char *new_value);
void remove_cache(Cache *cache, const char *key);

#endif