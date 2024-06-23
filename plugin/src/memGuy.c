#include "plugin.h"

static char *MODULE_NAME = "MemoryGuy";

void cleanup_func()
{
    plugin_printf("cleanup\n");
    return;
}

void router_callback(struct mg_connection *c, struct mg_http_message *hm)
{
    char uri_exp[16];
    sprintf(uri_exp, "%s/mem", MODULE_NAME);

    if (mg_strcmp(hm->uri, mg_str(uri_exp)) == 0)
    {
        mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{\"mem_usage\": %m}", SYSTEM_STATE.mem_usage);
        return;
    }

    mg_http_reply(c, 200, "", "MemoryGuy: memory monitoring plugin for SuikaBlog");
}

__attribute__((unused)) void load_module(void *module_handler)
{
    Plugin cur = {
        .name = MODULE_NAME,
        .version = PLUGIN_MANAGER_VERSION,
        .handler = module_handler,
        .cleanup_func = cleanup_func,
        .router_callback = router_callback,
    };

    register_plugin(cur);
}
