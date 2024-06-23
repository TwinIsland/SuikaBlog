#include <dlfcn.h>
#include <dirent.h>
#include <string.h>

#include "config.h"
#include "plugin.h"
#include "utils.h"
#include "sqlite3.h"
#include "db.h"

static Plugin plugins[10];
static int plugin_count = 0;

/*
    PlugIn Loader
*/
void load_plugins()
{
    const char *plugins_dir = config.plugin_dir;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(plugins_dir)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (!strstr(ent->d_name, ".suika"))
                continue;

            char plugin_path[512];
            snprintf(plugin_path, sizeof(plugin_path), "%s/%s", plugins_dir, ent->d_name);

            void *handler = dlopen(plugin_path, RTLD_LAZY);
            if (!handler)
            {
                debug("Could not load plugin: %s\n", dlerror());
                continue;
            }

            // Attempt to locate the registration function
            void (*register_plugin_func)() = (void (*)())dlsym(handler, "load_module");
            const char *dlsym_error = dlerror();
            if (dlsym_error)
            {
                debug("Warning: could not find load_module function in plugin: %s\n", dlsym_error);
                dlclose(handler);
                continue;
            }
            else
            {
                register_plugin_func(handler);
            }
        }
        closedir(dir);
    }
    else
        PRINT_ERR("Could not open plugins directory");
}

void unload_plugins()
{
    for (int i = 0; i < plugin_count; ++i)
    {
        if (plugins[i].cleanup_func)
        {
            debug("clean up %s", plugins[i].name);
            plugins[i].cleanup_func();
        }

        dlclose(plugins[i].handler);
    }
    plugin_count = 0;
}

/*
    Plugin Utils
*/

int register_plugin(Plugin plugin)
{
    if (plugin.version != PLUGIN_MANAGER_VERSION)
    {
        PRINT_ERR("%s version mistached", plugin.name);
        return -1;
    }
    if (plugin_count < (sizeof(plugins) / sizeof(Plugin)))
    {
        plugins[plugin_count] = plugin;
        return plugin_count++;
    }
    else
        return -1;
}

Result push_data(const char *key, const char *value)
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
        return (Result){.status = FAILED, .msg = "Failed to prepare statement"};

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

Result get_data(const char *key)
{
    char *value;
    if (db == NULL)
        return (Result){
            .status = FAILED,
            .msg = "uninitialized database connection",
        };
    const char *sql = "SELECT Value FROM Info WHERE Key = ?";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return (Result){.status = FAILED, .msg = "Failed to prepare statement"};

    sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const unsigned char *val = sqlite3_column_text(stmt, 0);
        value = val ? strdup((const char *)val) : NULL;
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