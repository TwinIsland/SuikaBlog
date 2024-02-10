#include <stdio.h>
#include <string.h>

#include "result.h"
#include "ini_handler.h"
#include "fs_helper.h"
#include "blog_init.h"
#include "utils.h"
#include "crud.h"
#include "ipc.h"

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

    PRINT_LOG("init: %s", ret, 0, config->db_name);
    PRINT_LOG("init: passkey", ret, 0);

    // init_ipc(config);
    // send_ipc("hello world");
    // sleep(100);
    // cleanup_ipc();

    return 0;
}