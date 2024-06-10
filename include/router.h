#ifndef ROUTER_H_
#define ROUTER_H_

#include "sha256.h"

void server_fn(struct mg_connection *c, int ev, void *ev_data);

struct user
{
  const char *name, *pass, *token;
};

#define ROUTER(router_name) void router_##router_name(struct mg_connection *c, int ev, void *ev_data, \
struct mg_http_message *hm, struct mg_http_serve_opts opts)

#define USE_ROUTER(router_name) router_##router_name(c, ev, ev_data, hm, opts)

#endif /* !ROUTER_H_ */
