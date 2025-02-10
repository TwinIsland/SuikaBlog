#pragma once

#include <sqlite3.h>

#include "result.h"

Result db_init();
void db_close();

extern sqlite3 *db;
