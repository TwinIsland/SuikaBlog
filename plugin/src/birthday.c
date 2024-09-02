#include "plugin.h"
#include <time.h>

static char *MODULE_NAME = "Birthday";

static char *birthday = NULL;
static const char *err_msg = NULL;

void cleanup_func()
{
    if (birthday)
        free(birthday);

    plugin_printf("cleanup\n");
    return;
}

void router_callback(struct mg_connection *c, struct mg_str cap)
{
    if (is_uri(cap, "get"))
    {
        if (!birthday)
        {
            mg_http_reply(c, 501, "", err_msg ? err_msg : "");
            return;
        }

        mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{\"birthday\": %s}", birthday ? birthday : "-1");
        return;
    }

    mg_http_reply(c, 200, "", "Birthday: record the birthday of your blog");
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

    if (get_info("birthday", &birthday).status == FAILED)
    {
        time_t now;
        time(&now);

        char timestamp[20];
        snprintf(timestamp, sizeof(timestamp), "%ld", now);
        push_info("birthday", timestamp);
        birthday = malloc(20);
        strcpy(birthday, timestamp);
    }
    else
    {
        Result ret;
        ret = get_info("birthday", &birthday);

        if (ret.status == FAILED)
            err_msg = ret.msg;
    }
}
