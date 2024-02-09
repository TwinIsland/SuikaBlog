#include <stdio.h>
#include <stdlib.h>

#include "crud.h"
#include "utils.h"

static sqlite3 *db = NULL;

Result db_init(const char *db_name)
{
    int rc = sqlite3_open(db_name, &db);
    if (rc != SQLITE_OK)
    {
        debug("Can't open database: %s\n", sqlite3_errmsg(db));
        return (Result){
            .status = FAILED,
            .msg = "Can't open database",
        };
    }
    return (Result){
        .status = OK,
    };
}

Result create_post(const char *title, const char *excerpts, const char *content)
{

    long long int post_id;
    char post_id_buf[256];
    const char *sql = "INSERT INTO Posts (Title, Excerpts, Content) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        debug("Preparation failed: %s\n", sqlite3_errmsg(db));
        return (Result){
            .status = FAILED,
            .msg = "sql query failed at preparation",
        };
    }

    // Bind the values to the statement
    sqlite3_bind_text(stmt, 1, title, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, excerpts, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, content, -1, SQLITE_STATIC);

    // Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        debug("Execution failed: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        post_id = sqlite3_last_insert_rowid(db);
        sprintf(post_id_buf, "%lld", post_id);
        debug("Post created successfully\n");
    }

    // Finalize the statement to prevent memory leaks
    sqlite3_finalize(stmt);

    return (Result){
        .status = rc == SQLITE_DONE ? OK : FAILED,
        .msg = rc == SQLITE_DONE ? post_id_buf : "sql query failed at execution",
    };
}

// Close the database connection
void db_close(void)
{
    if (db != NULL)
    {
        sqlite3_close(db);
        db = NULL;
    }
}