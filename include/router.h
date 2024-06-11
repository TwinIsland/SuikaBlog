#ifndef ROUTER_H_
#define ROUTER_H_

#include "sha256.h"

void server_fn(struct mg_connection *c, int ev, void *ev_data);

struct user
{
  const char *name, *pass, *token;
};

#define ROUTER(router_name, ...) void router_##router_name(struct mg_connection *c, int ev, void *ev_data, \
                                                           struct mg_http_message *hm, struct mg_http_serve_opts opts, ##__VA_ARGS__)

#define USE_ROUTER(router_name, ...) router_##router_name(c, ev, ev_data, hm, opts, ##__VA_ARGS__)

// print router error
#define ROUTER_ERR(router_name, ret, body)     \
  do                                               \
  {                                                \
    body = strdup("failed");                       \
    printf("\033[0;33m");                          \
    printf("/api/" router_name ": %s\n", ret.msg); \
    printf("\033[0m");                             \
    body = strdup("failed");                       \
  } while (0)

#define MAGIC_NUM 114514

#endif /* !ROUTER_H_ */
