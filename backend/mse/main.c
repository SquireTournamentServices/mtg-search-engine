#include <stdio.h>
#include <time.h>
#include <string.h>
#include "./mse.h"
#include "./save.h"
#include "../testing_h/testing.h"

#define MSE_HELP_ARG "--help"
#define MSE_GENERATE_ARG "--generate"

int main(int argc, char **argv)
{
    int exit_after_startup = 0;

    if (argc == 2) {
        if (strcmp(argv[1], MSE_HELP_ARG) == 0) {
            printf("Usage: %s\tOPTION?\n", argv[0]);
            printf("OPTIONS:\n);");
            printf("\t%s\tShows Help\n", MSE_HELP_ARG);
            printf("\t%s\tGenerates %s\n", MSE_GENERATE_ARG, MSE_CARDS_FILE_NAME);
            printf("\t(no options)\tLaunches in an interactive query mode\n");
            return 1;
        } else if (strcmp(argv[1], MSE_GENERATE_ARG) == 0) {
            exit_after_startup = 1;
        } else {
            printf("Invalid usage, use %s.", MSE_HELP_ARG);
            return 1;
        }
    } else if (argc > 2) {
        printf("Too many arguments, use %s.", MSE_HELP_ARG);
        return 1;
    }

    mse_t state;
    if (!mse_init(&state)) {
        lprintf(LOG_ERROR, "Cannot init state\n");
        exit(1);
    }

    if (exit_after_startup) {
        return 0;
    }

    char buffer[1024];
    for (; printf(ANSI_YELLOW "query > " ANSI_RESET), fgets(buffer, sizeof(buffer), stdin);) {
        size_t len = strlen(buffer);
        if (buffer[len - 1] == '\n') {
            buffer[len - 1] = 0;
        }

        mse_search_result_t res;
        if (!mse_search(&state, &res, buffer)) {
            lprintf(LOG_ERROR, "Cannot execute query\n");
        } else {
            mse_free_search_results(&res);
        }
    }

    mse_free(&state);
    return 0;
}
