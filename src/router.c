#include "mongoose.h"
#include "router.h"
#include "utils.h"
#include "fs_helper.h"

static configuration *config;
static const char *cached_exts[] = {".png", ".jpg", ".jpeg", ".webp", ".gif", ".svg", ".js", ".css", ".ttf", NULL};

void init_router(configuration *_config)
{
  config = _config;
}

// Parse HTTP requests, return authenticated user or NULL
// static struct user *getuser(struct mg_http_message *hm)
// {
//   // // In production, make passwords strong and tokens randomly generated
//   // // In this example, user list is kept in RAM. In production, it can
//   // // be backed by file, database, or some other method.

//   // char user[256], pass[256];
//   // struct user *u;
//   // mg_http_creds(hm, user, sizeof(user), pass, sizeof(pass));
//   // if (user[0] != '\0' && pass[0] != '\0')
//   // {
//   //   // Both user and password are set, search by user/password
//   //   if (strcmp(user, config->admin_name) == 0 && strcmp(pass, config->pass_sha256) == 0)
//   //     return u;
//   // }
//   // else if (user[0] == '\0')
//   // {
//   //   // Only password is set, search by token
//   //   for (u = users; u->name != NULL; u++)
//   //     if (strcmp(pass, u->token) == 0)
//   //       return u;
//   // }
//   return NULL;
// }

// Connection event handler function
void server_fn(struct mg_connection *c, int ev, void *ev_data)
{
  if (ev == MG_EV_HTTP_MSG)
  {
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;    
    struct mg_http_serve_opts opts = {.root_dir = "theme", .page404 = "theme/index.html"};

    // Cache all image request
    char uri[hm->uri.len+1];
    strncpy(uri, hm->uri.ptr, hm->uri.len);
    uri[hm->uri.len] = '\0';

    debug("request: %s", uri);

    if (check_file_with_exts(uri, cached_exts)) {
      debug("cache file: %s", uri);
      opts.extra_headers = "Cache-Control: max-age=259200\n";
    }


    mg_http_serve_dir(c, hm, &opts); // Serve static files
  }
}
