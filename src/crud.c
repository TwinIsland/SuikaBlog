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
static void populate_postInfo_from_stmt(sqlite3_stmt *stmt, PostInfo *postInfo)
{
    postInfo->PostID = sqlite3_column_int(stmt, 0);

    const unsigned char *title = sqlite3_column_text(stmt, 1);
    postInfo->Title = strdup((char *)title);

    const unsigned char *banner = sqlite3_column_text(stmt, 2);
    postInfo->Banner = banner ? strdup((char *)banner) : NULL;

    const unsigned char *excerpts = sqlite3_column_text(stmt, 3);
    postInfo->Excerpts = excerpts ? strdup((char *)excerpts) : NULL;

    postInfo->IsPage = sqlite3_column_int(stmt, 5); 
    postInfo->CreateDate = (time_t)sqlite3_column_int64(stmt, 6);
    postInfo->DateModified = (time_t)sqlite3_column_int64(stmt, 7);
    postInfo->UpVoted = sqlite3_column_int(stmt, 8);
    postInfo->Views = sqlite3_column_int(stmt, 9);
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

        ret->IsPage = sqlite3_column_int(stmt, 5);
        ret->CreateDate = (time_t)sqlite3_column_int(stmt, 6);
        ret->DateModified = (time_t)sqlite3_column_int(stmt, 7);
        ret->UpVoted = sqlite3_column_int(stmt, 8);
        ret->Views = sqlite3_column_int(stmt, 9);
    }

    sqlite3_finalize(stmt);
    return (Result){
        .status = OK,
        .ptr = ret,
    };
}

Result create_post(const char *title, const char *excerpt, const char *banner , const char *content, int isPage, int *ret)
{
    if (db == NULL)
        return UNINITIALIZE_ERR;

    const char *sql = "INSERT INTO Posts (Title, Excerpt, Banner, Content, IsPage) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
        return PREPARATION_ERR;

    sqlite3_bind_text(stmt, 1, title, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, excerpt, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, banner, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, content, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, isPage);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return (Result){FAILED, sqlite3_errmsg(db)};
    }

    long long int post_id = sqlite3_last_insert_rowid(db);
    *ret = post_id;
    sqlite3_finalize(stmt);
    return (Result){OK, "Post created successfully"};
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
        char *cur_tag;
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            cur_tag = (char *)sqlite3_column_text(stmt, 0);
            ret->data[index++] = strdup(cur_tag);
        }
    }
    else
        return PREPARATION_ERR;

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

    if (sqlite3_prepare_v2(db, sqlYearCounts, -1, &stmtYearCounts, NULL) != SQLITE_OK)
        return PREPARATION_ERR;

    size_t archieveCount = 0;
    while (sqlite3_step(stmtYearCounts) == SQLITE_ROW)
    {
        archieveCount++;
    }

    ret->data = malloc(sizeof(Archieve) * archieveCount);
    ret->size = archieveCount;

    sqlite3_reset(stmtYearCounts);

    size_t archieveIndex = 0;
    while (sqlite3_step(stmtYearCounts) == SQLITE_ROW)
    {
        int year = atoi((char *)sqlite3_column_text(stmtYearCounts, 0));
        size_t postCount = sqlite3_column_int(stmtYearCounts, 1);

        char **articleTitles = malloc(sizeof(char *) * postCount);

        const char *sqlPostsByYear = "SELECT Title FROM Posts WHERE strftime('%Y', CreateDate) = ? ORDER BY CreateDate DESC";
        sqlite3_stmt *stmtPostsByYear;

        if (sqlite3_prepare_v2(db, sqlPostsByYear, -1, &stmtPostsByYear, NULL) != SQLITE_OK)
            return PREPARATION_ERR;

        sqlite3_bind_text(stmtPostsByYear, 1, (char *)sqlite3_column_text(stmtYearCounts, 0), -1, SQLITE_STATIC);

        size_t postIndex = 0;
        while (sqlite3_step(stmtPostsByYear) == SQLITE_ROW && postIndex < postCount)
        {
            const unsigned char *title = sqlite3_column_text(stmtPostsByYear, 0);
            articleTitles[postIndex] = title ? strdup((char *)title) : NULL;
            postIndex++;
        }
        sqlite3_finalize(stmtPostsByYear);

        ret->data[archieveIndex++] = (Archieve){.year = year, .articleCount = postCount, .ArticleTitles = articleTitles};
    }

    sqlite3_finalize(stmtYearCounts);
    return (Result){.status = OK, .ptr = ret};
}

Result get_index(IndexData *ret)
{
    if (db == NULL)
        return UNINITIALIZE_ERR;

    const char *sql = "SELECT * FROM Posts WHERE IsPage=0 ORDER BY CreateDate DESC LIMIT ?";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        return PREPARATION_ERR;
    }

    sqlite3_bind_int(stmt, 1, config.index_post_n + 1); // +1 for cover article

    ret->NormalArticleInfos.data = malloc(sizeof(PostInfo) * config.index_post_n);
    ret->NormalArticleInfos.size = 0;

    int is_cover_article_row = 1;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        if (is_cover_article_row)
        {
            is_cover_article_row = 0;
            populate_postInfo_from_stmt(stmt, &ret->CoverArticleInfo);
            continue;
        }
        if (ret->NormalArticleInfos.size < config.index_post_n)
        {
            populate_postInfo_from_stmt(stmt, &ret->NormalArticleInfos.data[ret->NormalArticleInfos.size++]);
        }
    }

    sqlite3_finalize(stmt);

    Result exe_ret = get_all_tags(&ret->Tags);
    if (exe_ret.status == FAILED)
    {
        free(ret->NormalArticleInfos.data);
        return exe_ret;
    }

    exe_ret = get_archieves(&ret->Archieves);
    if (exe_ret.status == FAILED)
    {
        free(ret->NormalArticleInfos.data);
        return exe_ret;
    }

    // TODO: hardcoding now, fetch from notice plugin in the future
    ret->Notice.title = strdup("Welcome");
    ret->Notice.content = strdup("Welcome to the SuikaBlog System!");

    return (Result){.status = OK, .ptr = ret};
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