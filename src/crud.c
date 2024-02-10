#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

Result get_post(const int32_t PostID, Post *ret)
{
    if (db == NULL)
        return (Result){
            .status = FAILED,
            .msg = "uninitialized database connection",
        };

    const char *sql = "SELECT PostID, Title, Excerpts, Content, strftime('%s', DatePublished), strftime('%s', DateModified), UpVoted, Views FROM Posts WHERE PostID = ?";
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
        strncpy(ret->Title, (char *)sqlite3_column_text(stmt, 1), sizeof(ret->Title) - 1);
        ret->Title[sizeof(ret->Title) - 1] = '\0'; // Ensure null termination

        const char *excerpts = (const char *)sqlite3_column_text(stmt, 2);
        ret->Excerpts = excerpts ? strdup(excerpts) : NULL;

        const char *content = (const char *)sqlite3_column_text(stmt, 3);
        ret->Content = content ? strdup(content) : NULL;

        ret->DatePublished = (time_t)sqlite3_column_int(stmt, 4);
        ret->DateModified = (time_t)sqlite3_column_int(stmt, 5);
        ret->UpVoted = sqlite3_column_int(stmt, 6);
        ret->Views = sqlite3_column_int(stmt, 7);

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

Result create_post(const char *title, const char *excerpts, const char *content)
{

    if (db == NULL)
        return (Result){
            .status = FAILED,
            .msg = "uninitialized database connection",
        };

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

Result get_total_post_count(int *ret) {
    const char *sql = "SELECT COUNT(*) FROM Posts";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            *ret = sqlite3_column_int(stmt, 0); 
        }
        sqlite3_finalize(stmt);
    } else {
        *ret = -1;
        debug("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
    }
    
    return (Result) {
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