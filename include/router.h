#pragma once

#include "mongoose.h"
#include "sha256.h"
#include "cache.h"

void server_fn(struct mg_connection *c, int ev, void *ev_data);

#define JSON_type 0
#define STRING_type 1

#define ROUTER(router_name, ...) void router_##router_name(struct mg_connection *c, int ev, void *ev_data, \
                                                           struct mg_http_message *hm, ##__VA_ARGS__)

#define USE_ROUTER(router_name, ...) router_##router_name(c, ev, ev_data, hm, ##__VA_ARGS__)

// standard router reply function, can be called by
// 1. ROUTER_reply(c, router_name, content, content_type, code)
// 2. ROUTER_reply(c, router_name, ERR_TYPE)
#define ROUTER_reply(...) ROUTER_reply_(__VA_ARGS__)

// standard router reply int function, can be called by
// ROUTER_reply(c, router_name, int_response)
#define ROUTER_reply_int(c, router_name, int_response)                              \
  mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{ %m:%m, %m:%d }\n", \
                MG_ESC("status"), MG_ESC("true"),                                   \
                MG_ESC("content"), int_response);

#define ROUTER_reply_(c, router_name, content, content_type, code)                           \
  do                                                                                         \
  {                                                                                          \
    if (code != 200)                                                                         \
    {                                                                                        \
      printf("\033[0;33m");                                                                  \
      printf("/api/%s: %s\n", router_name, content);                                         \
      printf("\033[0m");                                                                     \
    }                                                                                        \
    if (content_type == STRING_type)                                                         \
      mg_http_reply(c, code, "Content-Type: application/json\r\n", "{ %m:%s, %m:\"%s\" }\n", \
                    MG_ESC("status"), code == 200 ? "true" : "false",                        \
                    MG_ESC("content"), content != NULL ? content : "");                      \
    else if (content_type == JSON_type)                                                      \
      mg_http_reply(c, code, "Content-Type: application/json\r\n", "{ %m:%s, %m:%s }\n",     \
                    MG_ESC("status"), code == 200 ? "true" : "false",                        \
                    MG_ESC("content"), content != NULL ? content : "\"\"");                  \
  } while (0)

#define SERVERSIDE_ERR_CODE 501
#define UNAUTH_ERR_code 401
#define BADREQ_ERR_code 400
#define OK_CODE 200
#define NOFOUND_CODE 404

#define SERVERSIDE_ERR "server error", STRING_type, SERVERSIDE_ERR_CODE
#define UNAUTH_ERR "permission denied", STRING_type, UNAUTH_ERR_code
#define BADREQ_ERR "bad request", STRING_type, BADREQ_ERR_code
#define NOFOUND_ERR "no found", STRING_type, NOFOUND_CODE
#define RESPONSE_OK "ok", STRING_type, OK_CODE

#define SID_LENGTH 6

// Cache
extern Cache *cache;
