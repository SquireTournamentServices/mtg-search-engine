#include "./system_test.h"
#include "../mse/mse.h"
#include "../mse/save.h"
#include "../testing_h/testing.h"
#include <string.h>
#include <unistd.h>

static int test_valid_queries(mse_t *state)
{
    FILE *f = fopen("./valid_queries.txt", "r");
    ASSERT(f != NULL);

    int flag = 1;
    while (flag) {
        char buffer[1024];
        char *r = fgets(buffer, sizeof(buffer), f);

        // EOF
        if (r == NULL) {
            flag = 0;
            break;
        }
        // Empty string is also EOF
        if (buffer[0] == 0) {
            flag = 0;
            break;
        }

        // Strip the last char (\n)
        if (strlen(buffer) > 2) return 1;
        size_t last = strlen(buffer) - 1;
        if (buffer[last] == '\n') {
            buffer[last] = 0;
        }

        mse_search_result_t res;
        ASSERT(mse_search(state, &res, buffer));
        mse_sort_search_results(&res, MSE_SORT_POWER);
        mse_free_search_results(&res);
    }

    fclose(f);
    return 1;
}

static int test_invalid_queries(mse_t *state)
{
    FILE *f = fopen("./invalid_queries.txt", "r");
    ASSERT(f != NULL);

    int flag = 1;
    while (flag) {
        char buffer[1024];
        char *r = fgets(buffer, sizeof(buffer), f);

        // EOF
        if (r == NULL) {
            flag = 0;
            break;
        }
        // Empty string is also EOF
        if (buffer[0] == 0) {
            flag = 0;
            break;
        }

        // Strip the last char (\n)
        ASSERT(strlen(buffer) > 2);
        size_t last = strlen(buffer) - 1;
        if (buffer[last] == '\n') {
            buffer[last] = 0;
        }

        mse_search_result_t res;
        int parse_res = mse_search(state, &res, buffer);
        if (parse_res) {
            lprintf(LOG_ERROR, "Expected parse of %s to fail\n", buffer);
        }
        ASSERT(!parse_res);
        mse_free_search_results(&res);
    }

    fclose(f);
    return 1;
}

int test_system()
{
    // Test that it is downloaded
    ASSERT(remove(MSE_CARDS_FILE_NAME) == 0);
    FILE *f = fopen(MSE_CARDS_FILE_NAME, "rb");
    ASSERT(f == NULL);

    mse_t state;
    ASSERT(mse_init(&state));
    mse_free(&state);

    // Test that it is read from local file
    f = fopen(MSE_CARDS_FILE_NAME, "rb");
    ASSERT(f != NULL);
    fclose(f);

    ASSERT(mse_init(&state));

    lprintf(LOG_INFO, "Testing valid queries\n");
    ASSERT(test_valid_queries(&state));

    lprintf(LOG_INFO, "Testing invalid queries\n");
    ASSERT(test_invalid_queries(&state));
    mse_free(&state);
    return 1;
}
