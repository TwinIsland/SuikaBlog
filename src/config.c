#include "config.h"

#include "sha256.h"

configuration config;

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
    else if (MATCH("server", "ip"))
        pconfig->server_ip = strdup(value);
    else if (MATCH("server", "plugin_dir"))
        pconfig->plugin_dir = strdup(value);
    else if (MATCH("database", "db_name"))
        pconfig->db_name = strdup(value);
    else if (MATCH("ipc", "ipc_path"))
        pconfig->ipc_path = strdup(value);
    else if (MATCH("ipc", "ipc_size"))
        pconfig->ipc_size = atoi(value);
    else if (MATCH("blog", "index_post_n"))
        pconfig->index_post_n = atoi(value);
    else if (MATCH("blog", "index_update_n"))
        pconfig->index_update_n = atoi(value);
    else if (MATCH("blog", "max_file_size"))
        pconfig->max_file_size = atoi(value);
    else if (MATCH("blog", "upload_dir"))
        pconfig->upload_dir = strdup(value);
    else if (MATCH("blog", "upload_uri"))
        pconfig->upload_uri = strdup(value);
    else if (MATCH("blog", "cache_n"))
        pconfig->cache_n = atoi(value);
    else if (MATCH("blog", "max_like_count_per_time"))
        pconfig->max_like_count_per_time = atoi(value);

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

    return (Result){
        .status = OK,
        .ptr = &config,
    };
}

Result load_passcode_to_config()
{
    FILE *file = fopen(config.key_file, "rb");

    if (file == NULL)
        return (Result){
            .status = FAILED,
            .msg = "Failed to open file for reading",
        };

    fgets(config.pass_sha256, SHA256_HEX_LEN + 1, file);

    if (strlen(config.pass_sha256) != SHA256_HEX_LEN)
    {
        return (Result){
            .status = FAILED,
            .msg = "Failed to read complete passcode",
        };
    }

    fclose(file);

    return (Result){
        .status = OK,
        .msg = "Passcode loaded successfully",
    };
}

Result load_upload_uri_pattern_to_config()
{
    config.upload_uri_pattern = malloc(strlen(config.upload_dir) + 3);
    sprintf(config.upload_uri_pattern, "%s/#", config.upload_uri);

    return (Result){
        .status = OK,
        .msg = "upload pattern loaded successfully",
    };
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
    if (config.server_ip)
        free(config.server_ip);
    if (config.plugin_dir)
        free(config.plugin_dir);
    if (config.key_file)
        free(config.key_file);
    if (config.upload_dir)
        free(config.upload_dir);
    if (config.upload_uri)
        free(config.upload_uri);
    if (config.upload_uri_pattern)
        free(config.upload_uri_pattern);
}
