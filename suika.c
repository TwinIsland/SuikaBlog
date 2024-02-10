#include <stdio.h>
#include <string.h>

#include "result.h"
#include "ini_handler.h"
#include "fs_helper.h"
#include "blog_init.h"
#include "utils.h"
#include "crud.h"
#include "models.h"

#define ASCII_LOGO_PATH "assets/ascii_logo"

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

void free_config(configuration *config) {
    if (config->admin_email) free(config->admin_email);
    if (config->admin_name) free(config->admin_name);
    if (config->db_name) free(config->db_name);
    if (config->ipc_path) free(config->ipc_path);
    if (config->key_file) free(config->key_file);
}

int main()
{
    Result ret;

    // print welcome message
    print_logo();

    // load configurations
    ret = init_config();
    configuration *config = get_config();
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

    ret = get_post(1, &cur);
    PRINT_LOG("test: get_post", ret, 0);
    printf("content: %ld\n", cur.DatePublished);

    ret = get_total_post_count(&total_post);
    PRINT_LOG("test: get_total_post_count", ret, 0);
    printf("content: %d\n", total_post);
    destroy_post(&cur);    
    db_close();
#endif

    // free all heap resources
    free_config(config);

    return 0;
}