#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "mongoose.h"
#include "result.h"
#include "config_loader.h"
#include "fs_helper.h"
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
    PRINT_OK_LOG("init: signal handler");

    // load configurations
    ret = init_config();
    PRINT_LOG("loading config", ret, ERR_IS_CRITICAL);

    // checking necessary files
    if (!(file_exists(config.key_file) & file_exists(config.db_name)))
    {
        ret.status = FAILED;
        ret.msg = "blog system need to initialize";
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
    // test code
    // debug("make sure you set the passkey=test");

    // const char *exp_password = "test";
    // BYTE *exp_sha256 = get_sha256_encrypt((const BYTE *)exp_password);
    // debug("test matched %d", SHA256_PASS_MATCHED(exp_sha256, config->pass_sha256));
    // free(exp_sha256);

    // int new_postid;
    // ret = create_post("testTitle", "test excerpt", "hello world", IS_POST, &new_postid);
    // PRINT_LOG("test create post", ret, ERR_IS_CRITICAL, exit_handler);

    // Post test;
    // get_post(new_postid, &test);
    // debug("Content is: %s", test.Content);
    // free_post(&test);
    // delete_post_by_id(new_postid);
    // get_post(new_postid, &test);
    // debug("Content after delete is: %s", test.Content);
    // free_post(&test);
    // int total_count;
    // get_total_post_count(&total_count);
    // debug("total post count is: %d", total_count);

    // IndexData test;
    // ret = get_index(&test);
    // debug("status %d index archieve count: %lu", ret.status, test.Archieves.size);
    // free_indexData(&test);

    // exit_handler();
#endif

    // start the server
    sprintf(server_addr, "http://0.0.0.0:%d", config.server_port);

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