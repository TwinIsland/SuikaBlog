#include "plugin.h"

static char *MODULE_NAME = "ExamplePlugin";

void cleanup_func()
{
    plugin_printf("cleanup\n");
    return;
}

__attribute__((unused)) void load_module(void *module_handler)
{
    Plugin cur = {
        .name = MODULE_NAME,
        .version = PLUGIN_MANAGER_VERSION,
        .handler = module_handler,
        .cleanup_func = cleanup_func,
    };

    register_plugin(cur);
}
