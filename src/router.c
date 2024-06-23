#include "router.h"
#include "utils.h"
#include "crud.h"
#include "models.h"
#include "config_loader.h"

#include "cache.h"

Cache *cache;

static const char *cached_exts[] = {".png", ".jpg", ".jpeg", ".webp", ".gif", ".svg", ".js", ".css", ".ttf", NULL};

static int is_authorized(struct mg_http_message *hm)
{
  char header_auth_sha256[SHA256_HEX_LEN + 1] = {0};
  for (int i = 0; i < MG_MAX_HTTP_HEADERS; ++i)
  {
    if (!mg_strcmp(hm->headers[i].name, mg_str("SuikaToken")))
    {
      strncpy(header_auth_sha256, hm->headers[i].value.buf, SHA256_BLOCK_SIZE);
      break;
    }
  }
  int ret = SHA256_PASS_MATCHED(header_auth_sha256, config.pass_sha256);
  return ret;
}

static char *mg_str_without_paren(struct mg_str src)
{
  char *ret = malloc(src.len - 1);
  strncpy(ret, src.buf + 1, src.len - 2);
  ret[src.len - 2] = '\0';
  return ret;
}

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
  char *body;

  if ((body = Cache_lookup("tags")))
  {
    ROUTER_reply(c, "tags", body, JSON_type, OK_CODE);
    return;
  }

  Tags tags;
  Result ret = get_all_tags(&tags);

  if (ret.status == FAILED)
  {
    free_tags(&tags);
    ROUTER_reply(c, "tags", SERVERSIDE_ERR);

    return;
  }
  else
    body = tags_to_json(&tags);

  free_tags(&tags);
  ROUTER_reply(c, "tags", body, JSON_type, OK_CODE);
  Cache_add("tags", body, ALWAYS_IN_CACHE);
}

ROUTER(archieves)
{
  char *body;

  if ((body = Cache_lookup("archieves")))
  {
    ROUTER_reply(c, "archieves", body, JSON_type, OK_CODE);
    return;
  }

  Archieves archieves = {.data = NULL};
  Result ret = get_archieves(&archieves);

  if (ret.status == FAILED)
  {
    free_archieves(&archieves);
    ROUTER_reply(c, "archieves", ret.msg, STRING_type, SERVERSIDE_ERR_CODE);
    return;
  }
  else
    body = archieves_to_json(&archieves);

  free_archieves(&archieves);
  ROUTER_reply(c, "archieves", body, JSON_type, OK_CODE);
  Cache_add("archieves", body, ALWAYS_IN_CACHE);
}

ROUTER(index)
{
  char *body;

  if ((body = Cache_lookup("index")))
  {
    ROUTER_reply(c, "index", body, JSON_type, OK_CODE);
    return;
  }

  IndexData index_data = {.CoverArticleInfo.PostID = -1};
  Result ret = get_index(&index_data);

  if (ret.status == FAILED)
  {
    free_indexData(&index_data);
    ROUTER_reply(c, "index", ret.msg, JSON_type, SERVERSIDE_ERR_CODE);
    return;
  }
  else
    body = indexData_to_json(&index_data);

  free_indexData(&index_data);
  ROUTER_reply(c, "index", body, JSON_type, OK_CODE);
  Cache_add("index", body, ALWAYS_IN_CACHE);
}

ROUTER(post, const int32_t PostID)
{
  Post post = {.PostID = -1};
  Result ret = get_post(PostID, &post);

  if (ret.status == FAILED)
  {
    free_post(&post);
    ROUTER_reply(c, "post", ret.msg, STRING_type, SERVERSIDE_ERR_CODE);
    return;
  }

  if (post.PostID != -1)
  {
    char *body = post_to_json(&post);
    free_post(&post);
    ROUTER_reply(c, "post", body, JSON_type, OK_CODE);
    free(body);
  }
  else
  {
    ROUTER_reply(c, "post", NOFOUND_ERR);
  }
}

// fontend must take the responsibility to divide files into smaller chunks, see: https://mongoose.ws/documentation/#mg_http_upload
ROUTER(upload)
{
  if (!is_authorized(hm))
    ROUTER_reply(c, "upload", UNAUTH_ERR);
  else
    mg_http_upload(c, hm, &mg_fs_posix, config.upload_dir, config.max_file_size);
}

ROUTER(create_post)
{
  if (!is_authorized(hm))
  {
    ROUTER_reply(c, "create_post", UNAUTH_ERR);
    return;
  }

  struct mg_str data = hm->body;

  Post post = {.PostID = -1};
  size_t ofs = 0;
  struct mg_str key, val;

  int field_n = 0;

  while ((ofs = mg_json_next(data, ofs, &key, &val)) > 0)
  {
    debug("%.*s -> %.*s\n", (int)key.len, key.buf, (int)val.len, val.buf);
    if (mg_strcmp(key, mg_str("\"title\"")) == 0)
    {
      post.Title = mg_str_without_paren(val);
      field_n++;
    }
    else if (mg_strcmp(key, mg_str("\"excerpts\"")) == 0)
    {
      post.Excerpts = mg_str_without_paren(val);
      field_n++;
    }
    else if (mg_strcmp(key, mg_str("\"banner\"")) == 0)
    {
      post.Banner = mg_str_without_paren(val);
      field_n++;
    }
    else if (mg_strcmp(key, mg_str("\"content\"")) == 0)
    {
      post.Content = mg_str_without_paren(val);
      mg_json_unescape(mg_str(post.Content), post.Content, strlen(post.Content));

      field_n++;
    }
    else if (mg_strcmp(key, mg_str("\"is_page\"")) == 0)
    {
      mg_str_to_num(val, 10, (void *)&post.IsPage, sizeof(post.IsPage));
      field_n++;
    }
  }

  if (field_n == 5)
  {
    int ret_postid;
    char ret_postid_str[16];
    Result ret = create_post(post.Title, post.Excerpts, post.Banner, post.Content, post.IsPage, &ret_postid);
    if (ret.status == FAILED)
    {
      free_post(&post);
      ROUTER_reply(c, "create_post", ret.msg, STRING_type, SERVERSIDE_ERR_CODE);
      return;
    }
    sprintf(ret_postid_str, "%d", ret_postid);
    ROUTER_reply(c, "create_post", ret_postid_str, STRING_type, OK_CODE);
    remove_cache("index");
  }
  else
    ROUTER_reply(c, "create_post", BADREQ_ERR);

  free_post(&post);
}

ROUTER(delete_post)
{
  if (!is_authorized(hm))
  {
    ROUTER_reply(c, "delete_post", UNAUTH_ERR);
    return;
  }

  struct mg_str id = mg_http_var(hm->query, mg_str("id"));
  int postid;
  if (!mg_str_to_num(id, 10, (void *)&postid, sizeof(postid)))
  {
    ROUTER_reply(c, "delete_post", BADREQ_ERR);
    return;
  }
  Result ret;
  if (ret = delete_post_by_id(postid), ret.status == FAILED)
    ROUTER_reply(c, "delete_post", ret.msg, STRING_type, SERVERSIDE_ERR_CODE);
  else
  {
    ROUTER_reply(c, "delete_post", ret.msg, STRING_type, OK_CODE);
    remove_cache("index");
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
      else if (mg_match(hm->uri, mg_str("/api/post/*"), caps))
      {
        int32_t arg;
        if (mg_str_to_num(caps[0], 10, &arg, sizeof(int32_t)))
          USE_ROUTER(post, arg);
      }
      else if (mg_match(hm->uri, mg_str("/api/archieves"), NULL))
        USE_ROUTER(archieves);
      else if (mg_match(hm->uri, mg_str("/api/index"), NULL))
        USE_ROUTER(index);
      else if (mg_match(hm->uri, mg_str("/api/upload"), NULL))
        USE_ROUTER(upload);
      else if (mg_match(hm->uri, mg_str("/api/write"), NULL))
        USE_ROUTER(create_post);
      else if (mg_match(hm->uri, mg_str("/api/delete"), NULL))
        USE_ROUTER(delete_post);
      else
        goto default_router;
    }
    else
    default_router:
      USE_ROUTER(index_page);
  }
}