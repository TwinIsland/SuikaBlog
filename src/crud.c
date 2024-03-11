#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crud.h"
#include "utils.h"

#define PLUGIN_LOADER_ALLOWED
#include "plugin.h"


static sqlite3 *db = NULL;

Result init_db(configuration *config)
{
    int rc = sqlite3_open(config->db_name, &db);
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

Result init_plugin()
{
    if (db == NULL)
        return (Result){
            .status = FAILED,
            .msg = "please initialize db first",
        };

    return plugin_init(db);
}

static void cp_stmt_str(char **dist, sqlite3_stmt *stmt, int idx)
{
    const char *src = (const char *)sqlite3_column_text(stmt, idx);

    if (!src)
    {
        *dist = (char *)malloc(1);
        if (*dist)
            (*dist)[0] = '\0';
        return;
    }

    size_t srcLen = strlen(src) + 1;
    *dist = (char *)malloc(srcLen);
    if (!*dist)
        return;
    strncpy(*dist, src, srcLen);
}

Result get_post(const int32_t PostID, Post *ret)
{
    if (db == NULL)
        return (Result){
            .status = FAILED,
            .msg = "uninitialized database connection",
        };

    const char *sql = "SELECT * FROM Posts WHERE PostID = ?";
    sqlite3_stmt *stmt;

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return (Result){
            .status = FAILED,
            .msg = "sql query failed at preparation",
        };

    // Bind the PostID to the SQL query
    sqlite3_bind_int(stmt, 1, PostID);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        ret->PostID = sqlite3_column_int(stmt, 0);

        cp_stmt_str(&ret->Title, stmt, 1);
        cp_stmt_str(&ret->Banner, stmt, 2);
        cp_stmt_str(&ret->Excerpts, stmt, 3);
        cp_stmt_str(&ret->Content, stmt, 4);

        ret->CreateDate = (time_t)sqlite3_column_int(stmt, 5);
        ret->DateModified = (time_t)sqlite3_column_int(stmt, 6);
        ret->UpVoted = sqlite3_column_int(stmt, 7);
        ret->Views = sqlite3_column_int(stmt, 8);

        sqlite3_finalize(stmt);
        return (Result){
            .status = OK,
            .ptr = ret,
        };
    }

    sqlite3_finalize(stmt);
    return (Result){
        .status = FAILED,
        .msg = "sql query failed at execution",
    };
}

Result create_post(const char *title, const char *excerpt, const char *content, int isPage, int *ret)
{
    if (db == NULL)
        return (Result){
            .status = FAILED,
            .msg = "uninitialized database connection",
        };

    long long int post_id;
    const char *sql = "INSERT INTO Posts (Title, Excerpt, Content, IsPage) VALUES (?, ?, ?, ?);";
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
    sqlite3_bind_text(stmt, 2, excerpt, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, content, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, isPage);

    // Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        debug("Execution failed: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        post_id = sqlite3_last_insert_rowid(db);
        *ret = post_id;
        debug("Post created successfully\n");
    }

    // Finalize the statement to prevent memory leaks
    sqlite3_finalize(stmt);

    return (Result){
        .status = rc == SQLITE_DONE ? OK : FAILED,
        .msg = rc == SQLITE_DONE ? "ok" : "sql query failed at execution",
    };
}

Result delete_post_by_id(long long int post_id)
{
    if (db == NULL)
        return (Result){
            .status = FAILED,
            .msg = "uninitialized database connection",
        };

    const char *sql = "DELETE FROM Posts WHERE PostID = ?;";
    sqlite3_stmt *stmt;

    // Prepare the SQL statement
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        debug("Preparation failed: %s\n", sqlite3_errmsg(db));
        return (Result){
            .status = FAILED,
            .msg = "sql query failed at preparation",
        };
    }

    // Bind the post_id to the statement
    sqlite3_bind_int64(stmt, 1, post_id);

    // Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        debug("Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt); // Finalize the statement to prevent memory leaks
        return (Result){
            .status = FAILED,
            .msg = "sql query failed at execution",
        };
    }

    // Finalize the statement to prevent memory leaks
    sqlite3_finalize(stmt);

    return (Result){
        .status = OK,
        .msg = "Post deleted successfully",
    };
}

Result get_total_post_count(int *ret)
{
    if (db == NULL)
        return (Result){
            .status = FAILED,
            .msg = "uninitialized database connection",
        };
    const char *sql = "SELECT COUNT(*) FROM Posts";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK)
    {
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            *ret = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    else
    {
        *ret = -1;
        debug("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
    }

    return (Result){
        .status = OK,
        .ptr = ret,
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