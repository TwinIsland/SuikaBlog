#include <stdio.h>
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
    
    printf("\nbye\n");
    exit(1);
}

int main()
{
    Result ret;
    char server_addr[32];

    // register signal handler
    signal(SIGINT, exit_handler);
    PRINT_LOG("init: signal handler", (Result){.status = OK}, 0);

    // print welcome message
    print_logo();

    // load configurations
    ret = init_config();
    config = get_config();

    PRINT_LOG("loading config", ret, 1);

    // checking config files
    if (!(file_exists(config->key_file) & file_exists(config->db_name)))
    {
        ret.status = FAILED;
        ret.msg = "blog system need to initialize";
        PRINT_LOG("loading system files", ret, 0);

        initialize_blog(config);
        printf("initialize end, please restart the program...\n");
        exit(1);
    }

    PRINT_LOG("init: %s", ret, 1, config->db_name);
    PRINT_LOG("init: passkey", ret, 1);

#ifdef TEST
    // test code
    db_init(config->db_name);
    Post cur;
    int total_post;

    ret = create_post("test title", "test excerpt", "test content");

    ret = get_post(1, &cur);
    PRINT_LOG("test: get_post", ret, 0);
    printf("content: %ld\n", cur.DatePublished);

    ret = get_total_post_count(&total_post);
    PRINT_LOG("test: get_total_post_count", ret, 0);
    printf("content: %d\n", total_post);
    destroy_post(&cur);
    db_close();
#endif

    // start the server
    sprintf(server_addr, "http://0.0.0.0:%d", config->server_port);

    mg_mgr_init(&mgr);
    mg_http_listen(&mgr, server_addr, (mg_event_handler_t)server_fn, NULL);

    PRINT_LOG("boost: server", (Result){.status = OK}, 0);
    printf("\nServer start at %s, enjoy!\n", server_addr);

    for (;;)
        mg_mgr_poll(&mgr, 1000); // Infinite event loop

    exit_handler();

    return 0;
}