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

#ifndef DEBUG
    if (check_file_with_exts(uri, cached_exts)) {
      debug("cache file: %s", uri);
      opts.extra_headers = "Cache-Control: max-age=259200\n";
    }
#else
    (void) cached_exts; // disable warning
#endif


    mg_http_serve_dir(c, hm, &opts); // Serve static files
  }
}
