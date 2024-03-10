#ifndef INI_HANDLER_H
#define INI_HANDLER_H

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
    char *db_name;
    char *ipc_path;
    int ipc_size;
    BYTE *pass_sha256;
} configuration;

// initialize config handler
Result init_config();

// get the config
configuration *get_config();

// config destoryer
void destory_config();

// load the local passcode to the config
Result load_passcode_to_config(configuration *old_config);

#endif