#pragma once
#include "../mongoose/mongoose.h"
#include <mse/mse.h>

typedef struct mse_web_server_t {
    mse_t mse;
    char *bind_addr;
} mse_web_server_t;

void mse_serve(mse_web_server_t *server);
