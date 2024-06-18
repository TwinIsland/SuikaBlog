#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "mongoose.h"
#include "result.h"
#include "config_loader.h"
#include "blog_init.h"
#include "utils.h"
#include "crud.h"
#include "models.h"
#include "router.h"
#include "suika_state.h"

#define PLUGIN_LOADER_ALLOWED
#include "plugin.h"

#define ASCII_LOGO_PATH "assets/ascii_logo"

static struct mg_mgr mgr;

// initialize system state
SUIKA_STATE SYSTEM_STATE = (SUIKA_STATE){
    .is_db_first_initialize = false,
};

void print_logo()
{
    FILE *file = fopen(ASCII_LOGO_PATH, "r");
    if (!file)
        goto print_welcome;

    int ch;
    while ((ch = fgetc(file)) != EOF)
        putchar(ch);

    fclose(file);

print_welcome:
    printf("\n\nWelcome to Suika Blog System!\n\n");
}

void exit_handler()
{
    // clean config
    destory_config();
    // clean server
    mg_mgr_free(&mgr);
    // close db
    db_close();
    // upload plugins
    unload_plugins();

    printf("\nbye\n");
    exit(1);
}

int main()
{

#ifndef DEBUG
    mg_log_set(0); // disable moogoose logging
#endif

    Result ret;
    char server_addr[32];

    // print welcome message
    print_logo();

    // register signal handler
    signal(SIGINT, exit_handler);
    signal(SIGTERM, exit_handler);
    PRINT_OK_LOG("init: signal handler");

    // load configurations
    ret = init_config();
    PRINT_LOG("loading config", ret, ERR_IS_CRITICAL);


    // loading uploading directory
    if (access(config.upload_dir, F_OK)) {
        if (mkdir(config.upload_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
            ret.status = FAILED;
            ret.msg = strerror(errno);
        }
    }
    PRINT_LOG("loading upload dir", ret, ERR_IS_CRITICAL);

    // checking necessary files
    if (access(config.key_file, F_OK) || access(config.db_name, F_OK))
    {
        ret.status = FAILED;
        ret.msg = strerror(errno);
        PRINT_LOG("loading system files", ret, ERR_IS_IGN);

        initialize_blog();
        printf("initialize end, please restart the program...\n");
        exit(1);
    }

    // update passcode to config struct
    ret = load_passcode_to_config();
    PRINT_LOG("init: keypass", ret, ERR_IS_CRITICAL);

    // initialize the database
    ret = init_db();
    PRINT_LOG("init: %s", ret, ERR_IS_CRITICAL, config.db_name);

    // initialize the plugins
    ret = init_plugins();
    PRINT_LOG("init: plugins", ret, ERR_IS_IGN);

#ifdef TEST
// do something
#endif

    // start the server
    sprintf(server_addr, "http://127.0.0.1:%d", config.server_port);

    mg_mgr_init(&mgr);

    if (!mg_http_listen(&mgr, server_addr, (mg_event_handler_t)server_fn, NULL))
    {
        char err_text[64];
        sprintf(err_text, "can't listen on port %d", config.server_port);

        ret = (Result){
            .status = FAILED,
            .msg = err_text};
    }
    else
        ret = (Result){.status = OK};

    PRINT_LOG("boost: server", ret, ERR_IS_CRITICAL);
    printf("\nServer start at %s\n", server_addr);

    for (;;)
        mg_mgr_poll(&mgr, 1000); // Infinite event loop

    exit_handler();
    return 0;
}