#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crud.h"
#include "utils.h"
#include "config.h"
#include "db.h"

#define PLUGIN_LOADER_ALLOWED
#include "plugin.h"

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

Result push_info(const char *key, const char *value)
{
    if (db == NULL)
        return UNINITIALIZE_ERR;

    // Modified SQL to perform an upsert: update the Value if the Key already exists
    const char *sql = "INSERT INTO Info (Key, Value) VALUES (?, ?) "
                      "ON CONFLICT(Key) DO UPDATE SET Value = excluded.Value";

    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return PREPARATION_ERR;

    sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, value, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        debug("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return PREPARATION_ERR;
    }

    sqlite3_finalize(stmt);

    return (Result){.status = OK};
}

Result get_info(const char *key, char **value)
{
    if (db == NULL)
        return UNINITIALIZE_ERR;

    const char *sql = "SELECT Value FROM Info WHERE Key = ?";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return PREPARATION_ERR;

    sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const unsigned char *val = sqlite3_column_text(stmt, 0);
        *value = val ? strdup((const char *)val) : NULL;
    }
    else
    {
        fprintf(stderr, "Key not found or failed to retrieve value: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return PREPARATION_ERR;
    }

    sqlite3_finalize(stmt);
    return (Result){.status = OK};
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
    strcpy(postInfo->CreateDate, (char *)sqlite3_column_text(stmt, 6));
    strcpy(postInfo->DateModified, (char *)sqlite3_column_text(stmt, 7));
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
        strcpy(ret->CreateDate, (char *)sqlite3_column_text(stmt, 6));
        strcpy(ret->DateModified, (char *)sqlite3_column_text(stmt, 7));
        ret->UpVoted = sqlite3_column_int(stmt, 8);
        ret->Views = sqlite3_column_int(stmt, 9);
    }

    increase_view_count(PostID);

    sqlite3_finalize(stmt);
    return (Result){
        .status = OK,
        .ptr = ret,
    };
}

void increase_view_count(const int32_t PostID)
{
    if (db == NULL)
        return;

    const char *sql = "UPDATE Posts SET Views = Views + 1 WHERE PostID = ?";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return;

    sqlite3_bind_int(stmt, 1, PostID);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

Result create_post(const char *title, const char *excerpt, const char *banner, const char *content, int isPage, int *ret)
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
    if (rc != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        return (Result){FAILED, sqlite3_errmsg(db)};
    }

    long long int post_id = sqlite3_last_insert_rowid(db);
    *ret = post_id;
    sqlite3_finalize(stmt);

    return (Result){OK, "Post created successfully"};
}

Result delete_post_by_id(const int32_t PostID)
{
    if (db == NULL)
        return UNINITIALIZE_ERR;

    const char *sql = "DELETE FROM Posts WHERE PostID = ?;";
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

    sqlite3_bind_int64(stmt, 1, PostID);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        debug("Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return (Result){
            .status = FAILED,
            .msg = "sql query failed at execution",
        };
    }

    // Check if any row was deleted
    int changes = sqlite3_changes(db);
    if (changes == 0)
    {
        sqlite3_finalize(stmt);
        return (Result){
            .status = FAILED,
            .msg = "post id not found",
        };
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);

    return (Result){
        .status = OK,
        .msg = "Post deleted successfully",
    };
}

Result get_all_tags(Tags *ret)
{
    if (db == NULL)
        return UNINITIALIZE_ERR;

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
    if (db == NULL)
        return UNINITIALIZE_ERR;

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

    sqlite3_stmt *stmt;

    char *sql = "SELECT p.* \
                FROM Posts AS p\
                JOIN PostMeta AS pm ON p.PostID = pm.PostID\
                WHERE p.isPage = 0 AND pm.MetaName = \"selected\"\
                ORDER BY p.CreateDate DESC \
                LIMIT ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return PREPARATION_ERR;

    sqlite3_bind_int(stmt, 1, SELECT_ARTICLE_N);

    ret->SelectedArticleInfos.data = malloc(sizeof(PostInfo) * SELECT_ARTICLE_N);
    ret->SelectedArticleInfos.size = 0;

    while (ret->SelectedArticleInfos.size < SELECT_ARTICLE_N && sqlite3_step(stmt) == SQLITE_ROW)
    {
        populate_postInfo_from_stmt(stmt, &ret->SelectedArticleInfos.data[ret->SelectedArticleInfos.size++]);
    }

    sqlite3_finalize(stmt);

    sql = "SELECT * FROM Posts WHERE IsPage=0 ORDER BY CreateDate DESC LIMIT ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return PREPARATION_ERR;

    sqlite3_bind_int(stmt, 1, config.index_post_n);

    ret->NormalArticleInfos.data = malloc(sizeof(PostInfo) * config.index_post_n);
    ret->NormalArticleInfos.size = 0;

    while (ret->NormalArticleInfos.size < config.index_post_n && sqlite3_step(stmt) == SQLITE_ROW)
    {
        populate_postInfo_from_stmt(stmt, &ret->NormalArticleInfos.data[ret->NormalArticleInfos.size++]);
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

Result get_PostInfos_from_n(int n, PostInfos *ret)
{
    if (db == NULL)
        return UNINITIALIZE_ERR;

    const char *sql = "SELECT * FROM Posts WHERE IsPage=0 ORDER BY CreateDate DESC LIMIT ? OFFSET ?";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return PREPARATION_ERR;

    sqlite3_bind_int(stmt, 1, config.index_update_n);
    sqlite3_bind_int(stmt, 2, n);

    ret->data = malloc(sizeof(PostInfo) * config.index_update_n);
    ret->size = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW && ret->size < config.index_update_n)
    {
        populate_postInfo_from_stmt(stmt, &ret->data[ret->size++]);
    }

    sqlite3_finalize(stmt);

    if (ret->size < config.index_post_n)
    {
        ret->data = realloc(ret->data, sizeof(PostInfo) * ret->size);
    }

    return (Result){.status = OK, .ptr = ret};
}
