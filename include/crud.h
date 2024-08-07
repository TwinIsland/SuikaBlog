#pragma once

#include "sqlite3.h"
#include "result.h"
#include "models.h"

#define IS_PAGE 1
#define IS_POST 0

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

// Post
Result create_post(const char *title, const char *excerpt, const char *banner, const char *content, int isPage, int *ret);
Result get_post(const int32_t PostID, Post *ret);
Result delete_post_by_id(const int32_t PostID);
void increase_view_count(const int32_t PostID);

// Tag
Result get_all_tags(Tags *ret);

// Archieve
Result get_archieves(Archieves *ret);

// Index
Result get_index(IndexData *ret);
Result get_PostInfos_from_n(int n, PostInfos *ret);

// Info
Result get_info(const char *key, char **value);
Result push_info(const char *key, const char *value);

// Views
Result get_views();

// Close database
void db_close(void);
