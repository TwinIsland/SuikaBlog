#include "mongoose.h"
#include "router.h"

static configuration *config;

void init_router(configuration *_config)
{
  config = _config;
}

// Parse HTTP requests, return authenticated user or NULL
static struct user *getuser(struct mg_http_message *hm)
{
  // // In production, make passwords strong and tokens randomly generated
  // // In this example, user list is kept in RAM. In production, it can
  // // be backed by file, database, or some other method.

  // char user[256], pass[256];
  // struct user *u;
  // mg_http_creds(hm, user, sizeof(user), pass, sizeof(pass));
  // if (user[0] != '\0' && pass[0] != '\0')
  // {
  //   // Both user and password are set, search by user/password
  //   if (strcmp(user, config->admin_name) == 0 && strcmp(pass, config->pass_sha256) == 0)
  //     return u;
  // }
  // else if (user[0] == '\0')
  // {
  //   // Only password is set, search by token
  //   for (u = users; u->name != NULL; u++)
  //     if (strcmp(pass, u->token) == 0)
  //       return u;
  // }
  return NULL;
}

// Connection event handler function
void server_fn(struct mg_connection *c, int ev, void *ev_data)
{
  if (ev == MG_EV_HTTP_MSG)
  {
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;
    struct user *u = getuser(hm);
    if (u == NULL && mg_http_match_uri(hm, "/api/#"))
    {
      // All URIs starting with /api/ must be authenticated
      mg_http_reply(c, 403, "", "Denied\n");
    }
    else if (mg_http_match_uri(hm, "/api/data"))
    {
      mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                    "{%m:%m,%m:%m}\n", MG_ESC("text"), MG_ESC("Hello!"),
                    MG_ESC("data"), MG_ESC("somedata"));
    }
    else if (mg_http_match_uri(hm, "/api/login"))
    {
      mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                    "{%m:%m,%m:%m}\n", MG_ESC("user"), MG_ESC(u->name),
                    MG_ESC("token"), MG_ESC(u->token));
    }
    else
    {
      struct mg_http_serve_opts opts = {.root_dir = "theme_new"};
      mg_http_serve_dir(c, hm, &opts); // Serve static files
    }
  }
}
