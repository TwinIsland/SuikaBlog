#include "ini_handler.h"

static configuration config;
static int is_init = 0;

static int config_loader(void *user, const char *section, const char *name,
                         const char *value)
{
    configuration *pconfig = (configuration *)user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    if (MATCH("admin", "name"))
        pconfig->admin_name = strdup(value);
    else if (MATCH("admin", "email"))
        pconfig->admin_email = strdup(value);
    else if (MATCH("admin", "key_file"))
        pconfig->key_file = strdup(value);
    else if (MATCH("server", "port"))
        pconfig->server_port = atoi(value);
    else if (MATCH("database", "db_name"))
        pconfig->db_name = strdup(value);

    else
        return 0; // raise error is no such entry
    return 1;
}

Result init_config()
{
    if (ini_parse(CONFIG_DIR, config_loader, &config) < 0)
    {
        return (Result){
            .status = FAILED,
            .msg = "fail to read ini file",
        };
    }

    is_init = 1;
    return (Result) {
        .status = OK,
        .ptr = &config,
    };
}

configuration *get_config()
{
    if (!is_init) return NULL;

    return &config;
}