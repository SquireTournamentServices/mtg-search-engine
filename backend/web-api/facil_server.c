#include "./facil_server.h"
#include <mse/thread_pool.h>
#include <testing_h/testing.h>
#include <http.h>

FIOBJ HTTP_X_DATA;

void on_request(http_s *request)
{
    http_set_cookie(request, .name = "my_cookie", .name_len = 9, .value = "data",
                    .value_len = 4);
    http_set_header(request, HTTP_HEADER_CONTENT_TYPE,
                    http_mimetype_find("txt", 3));
    http_set_header(request, HTTP_X_DATA, fiobj_str_new("my data", 7));
    http_send_body(request, "Hello World!\r\n", 14);
}

#define PORT "4365"

void mse_serve_facil(mse_t *mse)
{
    lprintf(LOG_INFO, "Starting server on port %s using facil.io\n", PORT);
    HTTP_X_DATA = fiobj_str_new("X-Data", 6);
    http_listen(PORT, NULL, .on_request = on_request, .log = 1);
    fio_start(.threads = mse_num_threads());

    lprintf(LOG_ERROR, "Server terminated\n");
    fiobj_free(HTTP_X_DATA);
}
