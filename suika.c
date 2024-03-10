#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mongoose.h>
#include <signal.h>

#include "result.h"
#include "ini_handler.h"
#include "fs_helper.h"
#include "blog_init.h"
#include "utils.h"
#include "crud.h"
#include "models.h"
#include "router.h"

#define ASCII_LOGO_PATH "assets/ascii_logo"

static configuration *config;
static struct mg_mgr mgr;

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

    printf("\nbye\n");
    exit(1);
}

int main()
{

#ifndef DEBUG
    mg_log_set(0);    // disable moogoose logging 
#endif

    Result ret;
    char server_addr[32];

    // print welcome message
    print_logo();

    // register signal handler
    signal(SIGINT, exit_handler);
    PRINT_LOG("init: signal handler", (Result){.status = OK}, ERR_IS_IGN, exit_handler);

    // load configurations
    ret = init_config();
    config = get_config();
    PRINT_LOG("loading config", ret, ERR_IS_CRITICAL, exit_handler);

    // checking config files
    if (!(file_exists(config->key_file) & file_exists(config->db_name)))
    {
        ret.status = FAILED;
        ret.msg = "blog system need to initialize";
        PRINT_LOG("loading system files", ret, ERR_IS_IGN, exit_handler);

        initialize_blog(config, exit_handler);
        printf("initialize end, please restart the program...\n");
        exit(1);
    }

    // load passcode to config
    ret = load_passcode_to_config();
    PRINT_LOG("init: keypass", ret, ERR_IS_CRITICAL, exit_handler);

    // initialize the database
    ret = init_db(config);
    PRINT_LOG("init: %s", ret, ERR_IS_CRITICAL, exit_handler, config->db_name);

#ifdef TEST
    // test code
    debug("make sure you set the passkey=test");

    const char *exp_password = "test";
    BYTE *exp_sha256 = get_sha256_encrypt((const BYTE *)exp_password);
    debug("test matched %d", SHA256_PASS_MATCHED(exp_sha256, config->pass_sha256));
    free(exp_sha256);

    // ret = create_post("testTitle", "test excerpt", "hello world", 0);
    // PRINT_LOG("test create post", ret, ERR_IS_CRITICAL, exit_handler);
    // int32_t new_postid = atoi(ret.msg);
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

    // exit_handler();

#endif

    // start the server
    sprintf(server_addr, "http://0.0.0.0:%d", config->server_port);

    mg_mgr_init(&mgr);

    if (!mg_http_listen(&mgr, server_addr, (mg_event_handler_t)server_fn, NULL))
    {
        char err_text[64];
        sprintf(err_text, "can't listen on port %d", config->server_port);

        ret = (Result)
        {
            .status = FAILED,
            .msg = err_text
        };
    }
    else
        ret = (Result){.status = OK};

    PRINT_LOG("boost: server", ret, ERR_IS_CRITICAL, exit_handler);
    printf("\nServer start at %s\n", server_addr);

    for (;;)
        mg_mgr_poll(&mgr, 1000); // Infinite event loop

    exit_handler();
    return 0;
}