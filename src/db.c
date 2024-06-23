#include "db.h"
#include "config.h"
#include "utils.h"

sqlite3 *db = NULL;

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

void db_close()
{
    if (db != NULL)
    {
        sqlite3_close(db);
        db = NULL;
    }
}