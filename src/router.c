#include "mongoose.h"
#include "router.h"
#include "utils.h"
#include "fs_helper.h"
#include "crud.h"
#include "models.h"

static const char *cached_exts[] = {".png", ".jpg", ".jpeg", ".webp", ".gif", ".svg", ".js", ".css", ".ttf", NULL};

ROUTER(index_page)
{

  // Cache all image request
  char uri[hm->uri.len + 1];
  strncpy(uri, hm->uri.buf, hm->uri.len);
  uri[hm->uri.len] = '\0';

  debug("request: %s", uri);

#ifndef DEBUG
  if (check_file_with_exts(uri, cached_exts))
  {
    debug("cache file: %s", uri);
    opts.extra_headers = "Cache-Control: max-age=259200\n";
  }
#else
  (void)cached_exts; // disable warning
#endif

  mg_http_serve_dir(c, hm, &opts); // Serve static files
}

ROUTER(tags)
{
  Tags tags;
  char *body;
  Result ret = get_all_tags(&tags);

  if (ret.status == FAILED)
  {
    PRINT_LOG("/api/tags", ret, ERR_IS_IGN);
    body = strdup("failed");
  }
  else
    body = tags_to_json(&tags);

  free_tags(&tags);
  mg_http_reply(c, 200, "Content-Type: application/json\r\n", body);
  free(body);
}

// Connection event handler function
void server_fn(struct mg_connection *c, int ev, void *ev_data)
{
  struct mg_http_message *hm = (struct mg_http_message *)ev_data;
  struct mg_http_serve_opts opts = {.root_dir = "theme", .page404 = "theme/index.html"};

  // we only accept http request
  if (ev == MG_EV_HTTP_MSG)
  {
    if (mg_match(hm->uri, mg_str("/api#"), NULL))
    {
      if (mg_match(hm->uri, mg_str("/api/tags"), NULL))
        USE_ROUTER(tags);
    }
    USE_ROUTER(index_page);
  }
}