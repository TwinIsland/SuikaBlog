#pragma once

#include <stdio.h>

#include "result.h"
#include "suika_state.h"
#include "mongoose.h"

#define PLUGIN_MANAGER_VERSION 2

#ifdef PLUGIN_LOADER_ALLOWED
void load_plugins();
void unload_plugins();
#endif

#ifndef PLUGIN_LOADER_ALLOWED
typedef struct
{
    char *name;
    int version;
    void *handler;
    void (*cleanup_func)();
    void (*router_callback)(struct mg_connection *c, struct mg_http_message *hm);   // TODO
} Plugin;

// external helper functions

// check if http request is authorized
extern int is_authorized(struct mg_http_message *hm);

#define plugin_printf(__fmt, ...) printf("%s: " __fmt, MODULE_NAME, ##__VA_ARGS__)

// return the plugin id, -1 if failed
int register_plugin(Plugin plugin);

Result push_data(const char *key, const char *value);
Result get_data(const char *key);
#endif
