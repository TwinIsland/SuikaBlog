#ifndef CRUD_H
#define CRUD_H

#include "sqlite3.h"
#include "result.h"
#include "models.h"

// Initializes the database connection
Result db_init(const char *db_name);

// Creates a new post
Result create_post(const char *title, const char *excerpts, const char *content);

// Get a post via post ID
Result get_post(const int32_t PostID, Post *ret);

// Get the total post count
Result get_total_post_count();

// Close database
void db_close(void);

#endif // CRUD_H
