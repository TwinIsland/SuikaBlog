#include <dlfcn.h>  
#include <dirent.h> 
#include <string.h>

#include "plugin.h"
#include "utils.h"
#include "sqlite3.h"

static sqlite3 *db = NULL;

static Plugin plugins[10];
static int plugin_count = 0;

void register_plugin(Plugin plugin) {
    if (plugin_count < (sizeof(plugins) / sizeof(Plugin))) {
        plugins[plugin_count++] = plugin;
    } else {
        debug("Max plugins reached.\n");
    }
}

void load_plugins() {
    const char *plugins_dir = "./plugin"; // Directory where plugins are stored
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(plugins_dir)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (!strstr(ent->d_name, ".suika")) {
                continue;
            }

            char plugin_path[256];
            snprintf(plugin_path, sizeof(plugin_path), "%s/%s", plugins_dir, ent->d_name);

            void *handle = dlopen(plugin_path, RTLD_LAZY);
            if (!handle) {
                debug("Could not load plugin: %s\n", dlerror());
                continue;
            }

            // Attempt to locate the registration function
            void (*register_plugin_func)() = (void (*)())dlsym(handle, "load_module");
            const char *dlsym_error = dlerror();
            if (dlsym_error) {
                debug("Warning: could not find load_module function in plugin: %s\n", dlsym_error);
                dlclose(handle);
                continue;
            }

            register_plugin_func();
        }
        closedir(dir);
    } else {
        PRINT_ERR("Could not open plugins directory");
    }
}

Result plugin_init(sqlite3 *_db)
{
    db = _db;

    for (int i = 0; i < plugin_count; i++)
    {
        if (plugins[i].before_server_start)
        {
            plugins[i].before_server_start();
            PRINT_OK("plugin: %s init", plugins[i].name);
        }
    }
    return (Result){.status = db != NULL ? OK : FAILED};
}

Result upsert_info_entry(char *key, char *value)
{
    if (db == NULL)
        return (Result){
            .status = FAILED,
            .msg = "uninitialized database connection",
        };

    // Modified SQL to perform an upsert: update the Value if the Key already exists
    const char *sql = "INSERT INTO Info (Key, Value) VALUES (?, ?) "
                      "ON CONFLICT(Key) DO UPDATE SET Value = excluded.Value";

    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        debug("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return (Result){.status = FAILED, .msg = "Failed to prepare statement"};
    }

    sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, value, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        debug("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return (Result){.status = FAILED, .msg = "Failed to prepare statement"};
    }

    sqlite3_finalize(stmt);

    return (Result){.status = OK};
}

Result fetch_info_value_by_key(char *key, char **value)
{
    if (db == NULL)
        return (Result){
            .status = FAILED,
            .msg = "uninitialized database connection",
        };
    const char *sql = "SELECT Value FROM Info WHERE Key = ?";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        debug("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return (Result){.status = FAILED, .msg = "Failed to prepare statement"};
    }

    sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const unsigned char *val = sqlite3_column_text(stmt, 0);
        *value = val ? strdup((const char *)val) : NULL;
    }
    else
    {
        debug("Key not found or failed to retrieve value: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return (Result){.status = FAILED, .msg = "Failed to prepare statement"};
    }

    sqlite3_finalize(stmt);

    return (Result){
        .status = OK,
        .ptr = value,
    };
}