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
    else if (MATCH("ipc", "ipc_path"))
        pconfig->ipc_path = strdup(value);
    else if (MATCH("ipc", "ipc_size"))
        pconfig->ipc_size = atoi(value);

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
    return (Result){
        .status = OK,
        .ptr = &config,
    };
}

Result load_passcode_to_config(configuration *old_config)
{
    FILE *file = fopen(config.key_file, "rb");

    if (file == NULL)
        return (Result){
            .status = FAILED,
            .msg = "Failed to open file for reading",
        };

    BYTE *buf = malloc(sizeof(BYTE) * SHA256_BLOCK_SIZE);
    size_t bytes_read = fread(buf, sizeof(BYTE), SHA256_BLOCK_SIZE, file);
    fclose(file);

    if (bytes_read < SHA256_BLOCK_SIZE)
    {
        return (Result){
            .status = FAILED,
            .msg = "Failed to read complete passcode",
        };
    }

    config.pass_sha256 = buf;
    old_config = get_config();

    return (Result) {
        .status = OK,
        .msg = "Passcode loaded successfully",
    };
}

configuration *get_config()
{
    if (!is_init)
        return NULL;

    return &config;
}

void destory_config()
{
    if (config.admin_email)
        free(config.admin_email);
    if (config.admin_name)
        free(config.admin_name);
    if (config.db_name)
        free(config.db_name);
    if (config.ipc_path)
        free(config.ipc_path);
    if (config.key_file)
        free(config.key_file);
    if (config.pass_sha256)
        free(config.pass_sha256);
}
