#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crud.h"
#include "utils.h"
#include "config_loader.h"

#define PLUGIN_LOADER_ALLOWED
#include "plugin.h"

static sqlite3 *db = NULL;

Result init_db()
{
    int rc = sqlite3_open(config.db_name, &db);
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

Result init_plugins()
{
    if (db == NULL)
        return (Result){
            .status = FAILED,
            .msg = "please initialize db first",
        };

    load_plugins();
    return plugins_bind(db);
}

#define PREPARATION_ERR                           \
    (Result)                                      \
    {                                             \
        .status = FAILED,                         \
        .msg = "sql query failed at preparation", \
    }

#define UNINITIALIZE_ERR                            \
    (Result)                                        \
    {                                               \
        .status = FAILED,                           \
        .msg = "uninitialized database connection", \
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

// SQL query should be like: SELECT * FROM Posts...
static void populate_post_info_from_stmt(sqlite3_stmt *stmt, PostInfo *postInfo)
{
    postInfo->PostID = sqlite3_column_int(stmt, 0);

    const unsigned char *title = sqlite3_column_text(stmt, 1);
    postInfo->Title = title ? strdup((char *)title) : NULL;

    const unsigned char *banner = sqlite3_column_text(stmt, 2);
    postInfo->Banner = banner ? strdup((char *)banner) : NULL;

    const unsigned char *excerpts = sqlite3_column_text(stmt, 3);
    postInfo->Excerpts = excerpts ? strdup((char *)excerpts) : NULL;

    postInfo->IsPage = sqlite3_column_int(stmt, 4); // Fixed column index for IsPage
    postInfo->CreateDate = (time_t)sqlite3_column_int64(stmt, 5);
    postInfo->DateModified = (time_t)sqlite3_column_int64(stmt, 6);
    postInfo->UpVoted = sqlite3_column_int(stmt, 7);
    postInfo->Views = sqlite3_column_int(stmt, 8);
}

Result get_post(const int32_t PostID, Post *ret)
{
    if (db == NULL)
        return UNINITIALIZE_ERR;

    const char *sql = "SELECT * FROM Posts WHERE PostID = ?";
    sqlite3_stmt *stmt;

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return PREPARATION_ERR;

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
        return UNINITIALIZE_ERR;

    long long int post_id;
    const char *sql = "INSERT INTO Posts (Title, Excerpt, Content, IsPage) VALUES (?, ?, ?, ?);";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
        return PREPARATION_ERR;

    // Bind the values to the statement
    sqlite3_bind_text(stmt, 1, title, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, excerpt, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, content, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, isPage);

    // Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
        debug("Execution failed: %s\n", sqlite3_errmsg(db));

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
    }
    else
    {
        *ret = -1;
        return PREPARATION_ERR;
    }

    sqlite3_finalize(stmt);

    return (Result){
        .status = OK,
        .ptr = ret,
    };
}

Result get_all_tags(Tags *ret)
{
    const char *sqlTags = "SELECT Name FROM Meta WHERE Type = 'tag'";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sqlTags, -1, &stmt, NULL) == SQLITE_OK)
    {
        size_t tagsCount = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW)
            tagsCount++;

        sqlite3_reset(stmt);

        ret->data = malloc(sizeof(Tag) * tagsCount);
        ret->size = tagsCount;

        size_t index = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            ret->data[index++] = strdup((char *)sqlite3_column_text(stmt, 0));
        }
    }
    else
    {
        return PREPARATION_ERR;
    }

    sqlite3_finalize(stmt);
    return (Result){
        .status = OK,
        .ptr = ret,
    };
}

Result get_archieves(Archieves *ret)
{
    const char *sqlYearCounts = "SELECT strftime('%Y', CreateDate) AS Year, COUNT(*) FROM Posts GROUP BY Year ORDER BY Year DESC";
    sqlite3_stmt *stmtYearCounts;

    if (sqlite3_prepare_v2(db, sqlYearCounts, -1, &stmtYearCounts, NULL) == SQLITE_OK)
    {
        size_t archieveCount = 0;
        while (sqlite3_step(stmtYearCounts) == SQLITE_ROW)
            archieveCount++;

        ret->data = malloc(sizeof(Archieve) * archieveCount);
        ret->size = archieveCount;

        sqlite3_reset(stmtYearCounts);

        size_t archieveIndex = 0;
        while (sqlite3_step(stmtYearCounts) == SQLITE_ROW)
        {
            int year = atoi((char *)sqlite3_column_text(stmtYearCounts, 0));
            size_t postCount = sqlite3_column_int(stmtYearCounts, 1);

            PostInfo *posts = malloc(sizeof(PostInfo) * postCount);

            const char *sqlPostsByYear = "SELECT * FROM Posts WHERE strftime('%Y', CreateDate) = ? ORDER BY CreateDate DESC";
            sqlite3_stmt *stmtPostsByYear;

            if (sqlite3_prepare_v2(db, sqlPostsByYear, -1, &stmtPostsByYear, NULL) == SQLITE_OK)
            {
                sqlite3_bind_text(stmtPostsByYear, 1, (char *)sqlite3_column_text(stmtYearCounts, 0), -1, SQLITE_STATIC);

                size_t postIndex = 0;
                while (sqlite3_step(stmtPostsByYear) == SQLITE_ROW && postIndex < postCount)
                {
                    populate_post_info_from_stmt(stmtPostsByYear, &posts[postIndex++]);
                }
                sqlite3_finalize(stmtPostsByYear);
            }

            ret->data[archieveIndex++] = (Archieve){.year = year, .articleCount = postCount, .posts = posts};
        }
    }
    else
    {
        return PREPARATION_ERR;
    }

    sqlite3_finalize(stmtYearCounts);
    return (Result){
        .status = OK,
        .ptr = ret,
    };
}

Result get_index(IndexData *ret)
{
    if (db == NULL)
        return UNINITIALIZE_ERR;

    const char *sql = "SELECT * FROM Posts ORDER BY CreateDate DESC LIMIT ?";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, INDEX_DATA_POST_N + 1); // +1 to include the cover article

        int is_cover_article_row = 1;
        int normal_article_cont = 0;

        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            if (is_cover_article_row)
            {
                is_cover_article_row = 0;
                // initialize cover article body
                populate_post_info_from_stmt(stmt, &ret->CoverArticleInfo);
                continue;
            }
            // initialize normal articles
            populate_post_info_from_stmt(stmt, &ret->NormalArticleInfos[normal_article_cont++]);
        }
    }
    else
        return PREPARATION_ERR;

    Result exe_ret;

    exe_ret = get_all_tags(&ret->Tags);
    if (exe_ret.status == FAILED)
        PRINT_ERR("fail to fetch tags: %s", exe_ret.msg);

    exe_ret = get_archieves(&ret->Archieves);
    if (exe_ret.status == FAILED)
        PRINT_ERR("fail to fetch archieves: %s", exe_ret.msg);

    // TODO: hardcoding now, change later !!
    ret->Notice.title = strdup("Welcome");
    ret->Notice.content = strdup("Welcome to the SuikaBlog System!");

    sqlite3_finalize(stmt);

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