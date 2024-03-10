#include <sqlite3.h>

#include "blog_init.h"
#include "fs_helper.h"
#include "utils.h"

static Result initialize_pass(const char *key_file, const BYTE *keypass)
{
    BYTE *buf = get_sha256_encrypt(keypass);

    FILE *file = fopen(key_file, "wb");
    if (file == NULL)
    {
        free(buf);
        return (Result){
            .status = FAILED,
            .msg = "Failed to open file",
        };
    }

    size_t bytes_written = fwrite(buf, sizeof(BYTE), SHA256_BLOCK_SIZE, file);
    if (bytes_written < SHA256_BLOCK_SIZE)
    {
        free(buf);
        fclose(file);
        delete_file(key_file);
        return (Result){
            .status = FAILED,
            .msg = "Failed to write the full buffer to the file",
        };
    }

    free(buf);
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

// Function to get password input securely
static void get_password(BYTE *password)
{
    fgets((char *)password, MAX_PASS_LENGTH, stdin);

    char *newline = strchr((char *)password, '\n');
    if (newline)
        *newline = '\0';
}

void initialize_blog(configuration *config, void (*exit_handler)())
{
    int c;
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
            PRINT_LOG("init %s", ret, ERR_IS_CRITICAL, exit_handler, config->db_name);
        }

        // clear stdin
        while ((c = getchar()) != '\n' && c != EOF)
            ;
    }
    if (!file_exists(config->key_file))
    {
        BYTE password[MAX_PASS_LENGTH];

        printf("\nCreate key Pass: ");
        get_password(password);

        printf("\n\n");
        ret = initialize_pass(config->key_file, password);

        PRINT_LOG("init passcode", ret, ERR_IS_CRITICAL, exit_handler);
    }

    printf("\n----------------------\n");
}