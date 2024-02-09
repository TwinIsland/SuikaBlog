#ifndef CRUD_H
#define CRUD_H

#include "sqlite3.h"
#include "result.h"

// Initializes the database connection
Result db_init(const char *db_name);

// Creates a new post
Result create_post(const char *title, const char *excerpts, const char *content);

// Close database
void db_close(void);


#endif // CRUD_H
