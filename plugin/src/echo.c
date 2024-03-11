#define USE_PLUGIN_UTILS
#include "plugin.h"
#include <stdio.h>

static char *MODULE_NAME = "EchoPlugin";

void echo_before_server_start()
{
    return;
}

__attribute__((unused)) void load_module()
{
    Plugin cur = {
        .name = MODULE_NAME,
        .before_server_start = echo_before_server_start,
    };

    register_plugin(cur);
}
