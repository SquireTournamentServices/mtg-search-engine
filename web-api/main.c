#include "../testing_h/testing.h"
#include "./server.h"

int main(int argc, char **argv)
{
    lprintf(LOG_INFO, "Starting %s web api (%s)\n", MSE_PROJECT_NAME, MSE_PROJECT_VERSION);

    mse_web_server_t server;
    memset(&server, 0, sizeof(server));

    if (!mse_init(&server.mse)) {
        lprintf(LOG_ERROR, "Cannot init state\n");
        return 1;
    }

    lprintf(LOG_INFO, "Starting server with cards\n");
    mse_serve(&server);
    mse_free(&server.mse);
    return 1;
}
