#ifndef PLUGIN_H
#define PLUGIN_H

#include <stdio.h>

#include "result.h"
#include "suika_state.h"

#define PLUGIN_MANAGER_VERSION 1

#ifdef PLUGIN_LOADER_ALLOWED
Result plugins_bind(sqlite3 *db);
void load_plugins();
void unload_plugins();
#endif

typedef struct
{
    char *name;
    int version;
    void *handler;
    void (*cleanup_func)();
    void (*after_binding_func)();
} Plugin;

void register_plugin(Plugin plugin);

// Define interface for plugins
Result upsert_info_entry(char *key, char *value);
Result fetch_info_value_by_key(char *key, char **value);

#endif // PLUGIN_H