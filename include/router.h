#ifndef ROUTER_H_
#define ROUTER_H_

#include "ini_handler.h"
#include "sha256.h"


void server_fn(struct mg_connection *c, int ev, void *ev_data);

void init_router(configuration *config);

struct user
{
  const char *name, *pass, *token;
};

#endif /* !ROUTER_H_ */
