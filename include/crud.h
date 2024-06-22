#ifndef CRUD_H
#define CRUD_H

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

// Initializes the database connection
Result init_db();

// Plugin initialize function
Result init_plugins();

// Post
Result create_post(const char *title, const char *excerpt, const char *banner, const char *content, int isPage, int *ret);
Result get_post(const int32_t PostID, Post *ret);
Result delete_post_by_id(long long int post_id);

// Tag
Result get_all_tags(Tags *ret);

// Archieve
Result get_archieves(Archieves *ret);

// Index
Result get_index(IndexData *ret);

// Close database
void db_close(void);

#endif // CRUD_H
