#include "plugin.h"
#include <stdio.h>
#include "utils.h"

static char *MODULE_NAME = "ExamplePlugin";

void echo_before_bind_db()
{
    PRINT_OK("hello from plugin!");
    return;
}

void cleanup_func()
{
    PRINT_OK("example plugin cleanup");
    return;
}

__attribute__((unused)) void load_module(void *module_handler)
{
    Plugin cur = {
        .name = MODULE_NAME,
        .version = PLUGIN_MANAGER_VERSION,
        .handler = module_handler,
        .cleanup_func = cleanup_func,
        .after_binding_func = echo_before_bind_db,
    };

    register_plugin(cur);
}
