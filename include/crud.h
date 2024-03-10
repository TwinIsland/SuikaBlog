#ifndef CRUD_H
#define CRUD_H

#include "sqlite3.h"
#include "result.h"
#include "models.h"
#include "ini_handler.h"

// Initializes the database connection
Result init_db(configuration *config);

Result create_post(const char *title, const char *excerpt, const char *content, int isPage);
Result get_post(const int32_t PostID, Post *ret);
Result delete_post_by_id(long long int post_id);
Result get_total_post_count(int *ret);

// Close database
void db_close(void);

#endif // CRUD_H
