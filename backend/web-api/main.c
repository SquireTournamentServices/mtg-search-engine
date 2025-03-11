#include <testing_h/testing.h>
#include "./server.h"

int main(int argc, char **argv)
{
    lprintf(LOG_INFO, "Starting %s web api (%s)\n", MSE_PROJECT_NAME, MSE_PROJECT_VERSION);

    mse_t mse;
    if (!mse_init(&mse)) {
        lprintf(LOG_ERROR, "Cannot init state\n");
        return 1;
    }

    lprintf(LOG_INFO, "Starting server with cards\n");
    mse_serve(&mse);

    lprintf(LOG_INFO, "Freeing mse\n");
    mse_free(&mse);
    return 1;
}
