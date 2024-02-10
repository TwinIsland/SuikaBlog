#include "mongoose.h"

// Connection event handler function
void server_fn(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_HTTP_MSG) {  // New HTTP request received
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;  // Parsed HTTP request
    if (mg_http_match_uri(hm, "/api/hello")) {                        // REST API call?
      mg_http_reply(c, 200, "", "{%m:%d}\n", MG_ESC("status"), 1);    // Yes. Respond JSON
    } else {
      struct mg_http_serve_opts opts = {.root_dir = "theme"};  
      mg_http_serve_dir(c, hm, &opts);                     // Serve static files
    }
  }
}
