#include "router.h"
#include "utils.h"
#include "crud.h"
#include "models.h"
#include "config.h"

#include "cache.h"

#define PLUGIN_LOADER_ALLOWED
#include "plugin.h"

Cache *cache;

static const char *cached_exts[] = {".png", ".jpg", ".jpeg", ".webp", ".gif", ".svg", ".js", ".css", ".ttf", NULL};

static struct upload_session_t
{
  int is_working;
  int upload_size;
  char *tmp_upload_path;
} upload_session;

int is_authorized(struct mg_http_message *hm)
{
  char header_auth_sha256[SHA256_HEX_LEN + 1] = {0};
  for (int i = 0; i < MG_MAX_HTTP_HEADERS; ++i)
  {
    if (!mg_strcmp(hm->headers[i].name, mg_str("Suika-Token")))
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

ROUTER(serve_dir, struct mg_http_serve_opts *opts)
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
    opts->extra_headers = "Cache-Control: max-age=259200\n";
  }
#else
  (void)cached_exts; // disable warning
#endif

  mg_http_serve_dir(c, hm, opts); // Serve static files
}

ROUTER(serve_file, struct mg_http_serve_opts *opts)
{
  size_t upload_uri_len = strlen(config.upload_uri);

  if (hm->uri.len == upload_uri_len + 1)
  {
    mg_http_serve_file(c, hm, "", opts);
    return;
  }

  char uri[hm->uri.len + 1];
  strncpy(uri, hm->uri.buf, hm->uri.len);
  uri[hm->uri.len] = '\0';

#ifndef DEBUG
  if (check_file_with_exts(uri, cached_exts))
  {
    debug("cache file: %s", uri);
    opts->extra_headers = "Cache-Control: max-age=259200\n";
  }
#else
  (void)cached_exts; // disable warning
#endif

  size_t upload_dir_len = strlen(config.upload_dir);
  size_t uri_len = strlen(uri);

  char file_path_raw[upload_dir_len + uri_len + 2];
  sprintf(file_path_raw, "%s/%s", config.upload_dir, uri + upload_uri_len + 1);

  // Prepare file_path
  char file_path[upload_dir_len + uri_len + 2];
  if (mg_url_decode(file_path_raw, strlen(file_path_raw), file_path, sizeof(file_path) - 1, 0) < 0)
  {
    ROUTER_reply(c, "serve_file", BADREQ_ERR);
    return;
  }

  // Ensure null-termination
  file_path[sizeof(file_path) - 1] = '\0';
  debug("serve file: %s", file_path);

  mg_http_serve_file(c, hm, file_path, opts);
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
    ROUTER_reply(c, "index", ret.msg, STRING_type, SERVERSIDE_ERR_CODE);
    return;
  }
  else
    body = indexData_to_json(&index_data);

  free_indexData(&index_data);
  ROUTER_reply(c, "index", body, JSON_type, OK_CODE);
  Cache_add("index", body, ALWAYS_IN_CACHE);
}

ROUTER(postInfos)
{
  struct mg_str from_str = mg_http_var(hm->query, mg_str("from"));
  int offset;
  if (!mg_str_to_num(from_str, 10, &offset, sizeof(offset)))
  {
    ROUTER_reply(c, "postInfos", BADREQ_ERR);
    return;
  }

  Result ret;
  PostInfos post_infos;

  if (ret = get_PostInfos_from_n(offset, &post_infos), ret.status == FAILED)
  {
    free_PostInfos(&post_infos);
    ROUTER_reply(c, "postInfos", ret.msg, STRING_type, SERVERSIDE_ERR_CODE);
    return;
  }

  char *body = postInfos_to_json(&post_infos);
  free_PostInfos(&post_infos);
  ROUTER_reply(c, "postInfos", body, JSON_type, OK_CODE);
  free(body);
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

/*
  This router does not follow the standard response format, which is an exception because it
  involves modifying a lot of mongoose.

*/
ROUTER(upload)
{
  if (!is_authorized(hm))
  {
    mg_http_reply(c, 401, "", "unauthorized");
    return;
  }

  if (!upload_session.is_working)
  {
    char tmp_name[RANDOM_UPLOAD_SUFX_LENGTH + 1];
    mg_random_str(tmp_name, sizeof(tmp_name));
    upload_session.tmp_upload_path = create_upload_directory(tmp_name, config.upload_dir);
    if (!upload_session.tmp_upload_path)
    {
      mg_http_reply(c, 501, "", "fail to create dir");
      return;
    }
    upload_session.is_working = 1;
  }

  upload_session.upload_size = mg_http_upload(c, hm, &mg_fs_posix, upload_session.tmp_upload_path, config.max_file_size);

  debug("uploading: %d", upload_session.upload_size);
  if (upload_session.upload_size < 0)
  {
    debug("upload failed");
    unlink(upload_session.tmp_upload_path);
    free(upload_session.tmp_upload_path);
    upload_session.tmp_upload_path = NULL;
    upload_session.is_working = 0;
    return;
  }
}

ROUTER(upload_finalizer)
{
  if (!is_authorized(hm))
  {
    ROUTER_reply(c, "upload_finalizer", UNAUTH_ERR);
    return;
  }

  debug("processing upload finalizer");
  if (!upload_session.is_working)
  {
    ROUTER_reply(c, "", "already finalized", STRING_type, OK_CODE);
    return;
  }

  char file[MG_PATH_MAX];
  int ret = mg_http_get_var(&hm->query, "file", file, sizeof(file));

  if (ret <= 0)
  {
    ROUTER_reply(c, "upload_finalizer", BADREQ_ERR);
    return;
  }

  char *new_upload_path = create_upload_directory(file, config.upload_dir);

  ret = rename(upload_session.tmp_upload_path, new_upload_path);

  free(upload_session.tmp_upload_path);
  upload_session.tmp_upload_path = NULL;
  upload_session.is_working = 0;

  if (ret != 0)
  {
    ROUTER_reply(c, "upload_finalizer", "finalizer error", STRING_type, SERVERSIDE_ERR_CODE);
    free(new_upload_path);
    return;
  }

  ROUTER_reply(c, "upload_finalizer", new_upload_path, STRING_type, OK_CODE);
  free(new_upload_path);
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

ROUTER(auth)
{
  if (is_authorized(hm))
    ROUTER_reply(c, "auth", RESPONSE_OK);
  else
    ROUTER_reply(c, "auth", UNAUTH_ERR);
}

// Connection event handler function
void server_fn(struct mg_connection *c, int ev, void *ev_data)
{
  struct mg_http_message *hm = (struct mg_http_message *)ev_data;
  struct mg_str caps[3]; // router argument buffer

  static struct mg_http_serve_opts theme_opts = {.root_dir = "theme", .page404 = "theme/index.html"};
  static struct mg_http_serve_opts upload_opts = {};

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
      else if (mg_match(hm->uri, mg_str("/api/postInfos#"), NULL))
        USE_ROUTER(postInfos);
      else if (mg_match(hm->uri, mg_str("/api/write"), NULL))
        USE_ROUTER(create_post);
      else if (mg_match(hm->uri, mg_str("/api/delete"), NULL))
        USE_ROUTER(delete_post);
      else if (mg_match(hm->uri, mg_str("/api/upload/finalizer#"), NULL))
        USE_ROUTER(upload_finalizer);
      else if (mg_match(hm->uri, mg_str("/api/upload"), NULL))
        USE_ROUTER(upload);
      else if (mg_match(hm->uri, mg_str("/api/auth"), NULL))
        USE_ROUTER(auth);
      else
        goto default_router;
    }
    else
    {
      if (mg_match(hm->uri, mg_str(config.upload_uri_pattern), NULL))
        USE_ROUTER(serve_file, &upload_opts);
      else if (match_plugins_router(c, hm))
        debug("use plugin router");
      else
      default_router:
        USE_ROUTER(serve_dir, &theme_opts);
    }
  }
}