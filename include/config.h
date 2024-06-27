#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ini.h"
#include "result.h"
#include "result.h"
#include "sha256.h"

#define CONFIG_DIR "config.ini"

typedef struct
{
    char *admin_name;
    char *admin_email;
    char *key_file;
    int server_port;
    char *server_ip;
    char *plugin_dir;
    char *db_name;
    char *ipc_path;
    int ipc_size;
    int index_post_n;
    int index_update_n;
    int max_file_size;
    char *upload_dir;
    char *upload_uri;
    int cache_n;
    char pass_sha256[SHA256_HEX_LEN + 1];
    char *upload_uri_pattern;
} configuration;

extern configuration config;

// initialize config handler
Result init_config();

// config destoryer
void destory_config();

// load the local passcode to the config
Result load_passcode_to_config();

// update the upload uri pattern via upload_uri
Result load_upload_uri_pattern_to_config();
