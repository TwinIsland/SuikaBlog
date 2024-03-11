#ifndef CRUD_H
#define CRUD_H

#include "sqlite3.h"
#include "result.h"
#include "models.h"
#include "ini_handler.h"

#define IS_PAGE 1
#define IS_POST 0

// Initializes the database connection
Result init_db(configuration *config);

// Plugin initialize function
Result init_plugin();

// Post
Result create_post(const char *title, const char *excerpt, const char *content, int isPage, int *ret);
Result get_post(const int32_t PostID, Post *ret);
Result delete_post_by_id(long long int post_id);
Result get_total_post_count(int *ret);

// Index


// Close database
void db_close(void);

#endif // CRUD_H
