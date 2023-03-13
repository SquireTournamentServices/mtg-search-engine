#include "./mtg_json.h"
#include "./io_utils.h"
#include "./thread_pool.h"
#include "../testing_h/testing.h"
#include <stdio.h>
#include <unistd.h>
#include <curl/curl.h>
#include <jansson.h>

size_t __mtg_json_write_callback(char *ptr,
                                 size_t size,
                                 size_t nmemb,
                                 void *data)
{
    FILE *w = (FILE *) data;
    fwrite(ptr, size, nmemb, w);
    fflush(w);
    return size * nmemb;
}

static void __do_get_atomic_cards_curl(FILE *w)
{
    CURL *curl = curl_easy_init();
    if(curl) {
        CURLcode res;

        // Set timeouts
        size_t timeout = 60 * 60 * 4; // 4 hours max download time
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);

        // Set url, user-agent and, headers
        curl_easy_setopt(curl, CURLOPT_URL, ATOMIC_CARDS_URL);
        curl_easy_setopt(curl, CURLOPT_USE_SSL, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, MSE_PROJECT_NAME);
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

        // Set response write
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &__mtg_json_write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) w);

        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            lprintf(LOG_INFO, "Finished getting atomic cards\n");
        } else {
            lprintf(LOG_ERROR, "Cannot get atomic cards\n");
        }
    } else {
        lprintf(LOG_ERROR, "Cannot init curl :(\n");
    }
}

static void __get_atomic_cards_curl_thread(void *data, struct thread_pool_t *pool)
{
    FILE *w = (FILE *) data;
    __do_get_atomic_cards_curl(w);
    fclose(w);
}

int get_atomic_cards(mtg_atomic_cards_t *ret, thread_pool_t *pool)
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
    task_t task = {(void *) w, &__get_atomic_cards_curl_thread};
    ASSERT(task_queue_enqueue(&pool->queue, task));

    // Start reading the json in this thread
    json_error_t error;
    size_t flags = 0;
    json_t *json = json_loadf(r, flags, &error);
    int ret_code = 1;

    // If the json cannot be read due to an error, then the curl request failed, its thread should have cleared
    // that bit up
    if (json == NULL) {
        lprintf(LOG_ERROR, "Cannot parse atomic json, %100s\n", error.text);
        ret_code = 0;
        goto cleanup;
    }

    // This only runs if there was no error
    // TODO: process the json innit mate.
    json_decref(json);

cleanup:
    ;
    fclose(r);
    return ret_code;
}

void free_atomic_cards(mtg_atomic_cards_t *cards)
{

}
