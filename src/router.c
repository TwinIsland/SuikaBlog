#include "mongoose.h"
#include "router.h"
#include "utils.h"
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
    ROUTER_ERR("tags", ret, body);
  else
    body = tags_to_json(&tags);

  free_tags(&tags);
  mg_http_reply(c, 200, "Content-Type: application/json\r\n", body);
  free(body);
}

ROUTER(post, const int32_t PostID)
{
  Post post = {.PostID = -1};
  char *body = "";
  Result ret = get_post(PostID, &post);

  if (ret.status == FAILED)
  {
    ROUTER_ERR("post", ret, body);
    return;
  }

  if (post.PostID != -1)
  {
    body = post_to_json(&post);
    free_post(&post);
    mg_http_reply(c, 200, "Content-Type: application/json\r\n", body);
    free(body);
  }
  else
  {
    body = mg_mprintf("{%m: %m}", MG_ESC("error"), MG_ESC("Post not found"));
    mg_http_reply(c, 404, "Content-Type: application/json\r\n", body);
    free(body);
  }
}

// Connection event handler function
void server_fn(struct mg_connection *c, int ev, void *ev_data)
{
  struct mg_http_message *hm = (struct mg_http_message *)ev_data;
  struct mg_http_serve_opts opts = {.root_dir = "theme", .page404 = "theme/index.html"};
  struct mg_str caps[3]; // router argument buffer

  // we only accept http request
  if (ev == MG_EV_HTTP_MSG)
  {
    if (mg_match(hm->uri, mg_str("/api#"), NULL))
    {
      if (mg_match(hm->uri, mg_str("/api/tags"), NULL))
        USE_ROUTER(tags);
      if (mg_match(hm->uri, mg_str("/api/post/*"), caps))
      {
        int32_t arg;
        if (!mg_str_to_num(caps[0], 10, &arg, sizeof(int32_t)))
          goto default_router;
        USE_ROUTER(post, arg);
      }
    }
    else
    default_router:
      USE_ROUTER(index_page);
  }
}