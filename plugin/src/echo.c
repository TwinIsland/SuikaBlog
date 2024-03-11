#define USE_PLUGIN_UTILS
#include "plugin.h"
#include <stdio.h>

static char *MODULE_NAME = "EchoPlugin";
static int MODULE_VERSION = PLUGIN_MANAGER_VERSION;

void echo_before_server_start()
{
    return;
}

void cleanup()
{
    return;
}

__attribute__((unused)) void load_module(void *module_handler)
{
    Plugin cur = {
        .name = MODULE_NAME,
        .version = MODULE_VERSION,
        .handler = module_handler,
        .cleanup_func = cleanup,
        .after_binding_func = echo_before_server_start,
    };

    register_plugin(cur);
}
