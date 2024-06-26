#include <sqlite3.h>
#include <unistd.h>

#include "install.h"
#include "utils.h"
#include "config.h"

static Result initialize_pass(const char *key_file, const char *keypass)
{
    char *buf = get_sha256_hashed(keypass);

    FILE *file = fopen(key_file, "w");
    if (file == NULL)
    {
        free(buf);
        return (Result){
            .status = FAILED,
            .msg = "Failed to open file",
        };
    }

    fputs(buf, file);
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
static void get_password(char *password)
{
    fgets((char *)password, MAX_PASS_LENGTH, stdin);

    char *newline = strchr((char *)password, '\n');
    if (newline)
        *newline = '\0';
}

void initialize_blog()
{
    int c;
    char input;
    Result ret;

    printf("\n----------------------");

    if (access(config.db_name, F_OK))
    {
        printf("\ninitialize database? [Y/N]: ");
        scanf(" %c", &input);
        if (input == 'Y' || input == 'y')
        {
            ret = initialize_db(config.db_name);
            printf("\n");
            PRINT_LOG("init %s", ret, ERR_IS_CRITICAL, config.db_name);
        }

        // clear stdin
        while ((c = getchar()) != '\n' && c != EOF)
            ;
    }
    if (access(config.key_file, F_OK))
    {
        char password[MAX_PASS_LENGTH];

        printf("\nCreate key Pass: ");
        get_password(password);

        printf("\n\n");
        ret = initialize_pass(config.key_file, password);

        PRINT_LOG("init passcode", ret, ERR_IS_CRITICAL);
    }

    printf("\n----------------------\n");
}