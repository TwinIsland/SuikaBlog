#ifndef PLUGIN_H
#define PLUGIN_H

#include <stdio.h>

#include "result.h"
#include "suika_state.h"

#ifdef USE_PLUGIN_UTILS
#define PRINT_LOG(name, msg, ...)                      \
    do                                                 \
    {                                                  \
        printf("(%s) " msg "\n", name, ##__VA_ARGS__); \
    } while (0)
#endif

#ifdef PLUGIN_LOADER_ALLOWED
Result plugin_init(sqlite3 *db);
void load_plugins();
#endif

typedef struct
{
    char *name;
    void (*before_server_start)();
} Plugin;

void register_plugin(Plugin plugin);

// Define interface for plugins
Result upsert_info_entry(char *key, char *value);
Result fetch_info_value_by_key(char *key, char **value);

#endif // PLUGIN_H