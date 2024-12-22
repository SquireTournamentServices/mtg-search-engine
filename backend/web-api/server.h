#pragma once
#include "../mongoose/mongoose.h"
#include <mse/mse.h>

typedef struct mse_web_server_t {
    mse_t mse;
} mse_web_server_t;

void mse_serve(mse_t *mse_in);
