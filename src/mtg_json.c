#include "./mtg_json.h"
#include "./io_utils.h"
#include "../testing_h/testing.h"
#include <stdio.h>
#include <unistd.h>
#include <curl/curl.h>

int get_atomic_cards(mtg_json_query_result_t *ret, thread_pool_t *pool)
{
    // Create the pipe that will be used for IPC
    int fid[2];
    int c = pipe(fid);
    ASSERT(c == 0);

    FILE *r = fdopen(fid[0], "r");
    ASSERT(r != NULL);

    FILE *w = fdopen(fid[1], "w");
    ASSERT(w != NULL);

    // Start curl request that writes to a pipe in another thread

    // Start reading the json in this thread

    // If the json cannot be read due to an error, then the curl request failed, its thread should have cleared
    // that bit up

    fclose(r);

    return 1;
}
