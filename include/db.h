#pragma once

#include <sqlite3.h>

#include "result.h"

Result init_db();
void db_close();

extern sqlite3 *db;
