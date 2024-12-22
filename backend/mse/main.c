#include <stdio.h>
#include <time.h>
#include <string.h>
#include "./mse.h"
#include "../testing_h/testing.h"

int main()
{
    mse_t state;
    if (!mse_init(&state)) {
        lprintf(LOG_ERROR, "Cannot init state\n");
        exit(1);
    }

    char buffer[1024];
    for (; printf(ANSI_YELLOW "query > " ANSI_RESET), fgets(buffer, sizeof(buffer), stdin);) {
        mse_search_result_t res;
        if (!mse_search(&state, &res, buffer)) {
            lprintf(LOG_ERROR, "Cannot execute query\n");
        } else {
            mse_free_search_results(&res);
        }
    }

    mse_free(&state);
}
