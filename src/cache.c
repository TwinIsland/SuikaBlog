#include "cache.h"
#include "config.h"
#include "utils.h"

void initialize_Cache()
{
    cache = (Cache *)malloc(sizeof(Cache));
    cache->head = NULL;
    cache->tail = NULL;
    cache->size = 0;
}

void *Cache_lookup(const char *key)
{
    CacheEntry *current = cache->head;
    while (current)
    {
        if (strcmp(current->key, key) == 0)
        {
            debug("%s: cache hit", current->key);
            return current->value;
        }
        current = current->next;
    }
    return NULL;
}

// value deallocation will be handled by it
void Cache_add(const char *key, void *value, int always_in_flag, void (*destroyer)(void *value))
{
    CacheEntry *new_entry = (CacheEntry *)malloc(sizeof(CacheEntry));
    strncpy(new_entry->key, key, CACHE_KEY_MAX_SIZE);
    new_entry->value = value;
    new_entry->always_in_flag = always_in_flag;
    new_entry->destroyer = destroyer;
    new_entry->next = NULL;

    if (cache->size == config.cache_n)
    {
        // Evict the oldest entry (FIFO) if it's not always_in_flag
        CacheEntry *current = cache->head;
        CacheEntry *prev = NULL;
        while (current && current->always_in_flag)
        {
            prev = current;
            current = current->next;
        }
        if (current)
        {
            if (prev)
            {
                prev->next = current->next;
            }
            else
            {
                cache->head = current->next;
            }
            if (current == cache->tail)
            {
                cache->tail = prev;
            }
            free(current->value);
            free(current);
            cache->size--;
        }
    }

    if (cache->tail)
    {
        cache->tail->next = new_entry;
    }
    else
    {
        cache->head = new_entry;
    }
    cache->tail = new_entry;
    cache->size++;
}

void free_Cache()
{
    CacheEntry *current = cache->head;
    printf("free caches\n");
    while (current)
    {
        CacheEntry *next = current->next;
        if (current->value)
        {
            if (current->destroyer)
                current->destroyer(current->value);
            else
                free(current->value);
        }
        free(current);
        current = next;
    }
    free(cache);
}

void update_cache(const char *key, void *new_value)
{
    CacheEntry *current = cache->head;
    while (current)
    {
        if (strcmp(current->key, key) == 0)
        {
            if (current->destroyer)
                current->destroyer(current->value);
            else
                free(current);
            current->value = new_value;
            return;
        }
        current = current->next;
    }
}

void remove_cache(const char *key)
{
    CacheEntry *current = cache->head;
    CacheEntry *prev = NULL;
    while (current)
    {
        if (strcmp(current->key, key) == 0)
        {
            if (prev)
                prev->next = current->next;

            else
                cache->head = current->next;

            if (current == cache->tail)
                cache->tail = prev;

            if (current->destroyer)
                current->destroyer(current->value);
            else
                free(current);
            cache->size--;
            return;
        }
        prev = current;
        current = current->next;
    }
}
