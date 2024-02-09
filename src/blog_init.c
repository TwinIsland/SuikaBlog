#include <sqlite3.h>
#include <termios.h>

#include "blog_init.h"
#include "fs_helper.h"
#include "utils.h"


static Result initialize_pass(const char *key_file, const BYTE *pass)
{
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, pass, strlen((const char *)pass));
    BYTE buf[SHA256_BLOCK_SIZE];

    sha256_final(&ctx, buf);

    FILE *file = fopen(key_file, "wb");
    if (file == NULL)
    {
        return (Result){
            .status = FAILED,
            .msg = "Failed to open file",
        };
    }

    size_t bytes_written = fwrite(buf, sizeof(BYTE), SHA256_BLOCK_SIZE, file);
    if (bytes_written < SHA256_BLOCK_SIZE)
    {
        fclose(file);
        return (Result){
            .status = FAILED,
            .msg = "Failed to write the full buffer to the file",
        };
    }

    fclose(file);

    return (Result){
        .status = OK,
    };
}

static Result initialize_db(const char *db_path)
{
    sqlite3 *db;
    char *errMsg = NULL;
    int rc;

    // Open the SQLite database
    rc = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK)
    {
        sqlite3_close(db);
        return (Result){
            .status = FAILED,
            .msg = sqlite3_errmsg(db),
        };
    }

    // Read SQL commands from db_ini.sql file
    FILE *sqlFile = fopen(DB_INI_SCRIPT_PATH, "r");
    if (sqlFile == NULL)
    {
        sqlite3_close(db);
        return (Result){
            .status = FAILED,
            .msg = "Cannot open sql_ini file",
        };
    }

    // Read the content of the SQL file
    fseek(sqlFile, 0, SEEK_END);
    long fsize = ftell(sqlFile);
    fseek(sqlFile, 0, SEEK_SET);

    char *sql = malloc(fsize + 1);
    fread(sql, 1, fsize, sqlFile);
    fclose(sqlFile);
    sql[fsize] = 0;

    // Execute SQL commands
    rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
        debug("SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        sqlite3_close(db);
        free(sql);
        return (Result){
            .status = FAILED,
            .msg = "SQL error",
        };
    }

    // Clean up
    sqlite3_close(db);
    free(sql);

    return (Result){
        .status = OK,
    };
}

static void disable_echo()
{
    struct termios old_term, new_term;
    tcgetattr(STDIN_FILENO, &old_term);
    new_term = old_term;
    new_term.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
}

// Function to enable terminal echo
static void enable_echo()
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Function to get password input securely
static void get_password(BYTE *password)
{
    disable_echo();
    fgets((char *)password, MAX_PASS_LENGTH, stdin);
    enable_echo();
    // Remove trailing newline character, if any
}

void initialize_blog(configuration *config)
{
    char input;
    Result ret;

    printf("\n----------------------");

    if (!file_exists(config->db_name))
    {
        printf("\ninitialize database? [Y/N]: ");
        scanf(" %c", &input);
        if (input == 'Y' || input == 'y')
        {
            ret = initialize_db(config->db_name);
            printf("\n");
            PRINT_LOG("init %s", ret, 1, config->db_name);
        }
    }
    if (!file_exists(config->key_file))
    {
        int c;
        BYTE password[MAX_PASS_LENGTH];

        // clear stdin
        while ((c = getchar()) != '\n' && c != EOF)
            ;

        printf("\nCreate key Pass: ");
        get_password(password);

        printf("\n\n");
        ret = initialize_pass(config->key_file, password);
        PRINT_LOG("init passcode", ret, 1);
    }

    printf("\n----------------------\n");
}