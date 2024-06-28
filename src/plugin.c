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

int match_plugins_router(struct mg_connection *c, struct mg_http_message *hm)
{
    int match_flag = 0;
    char plugin_uri_pattern[64];
    struct mg_str caps[1];

    for (int i = 0; i < plugin_count; ++i)
    {
        sprintf(plugin_uri_pattern, "/plugin/%s/#", plugins[i].name);
        if (plugins[i].router_callback && mg_match(hm->uri, mg_str(plugin_uri_pattern), caps))
        {
            plugins[i].router_callback(c, caps[0]);
            match_flag = 1;
        }
    }
    return match_flag;
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
