#include "./mtg_json.h"
#include "./io_utils.h"
#include "./thread_pool.h"
#include "../testing_h/testing.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
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

#define CURL_ASSERT(x) if ((x) != CURLE_OK) { lprintf(LOG_ERROR, "Cannot perform operation %s on cURL request\n", #x); goto curl_set_error; }

static void __do_get_all_printings_cards_curl(FILE *w)
{
    CURL *curl = curl_easy_init();
    if(curl) {
        CURLcode res;

        // Set timeouts
        size_t timeout = 60 * 60 * 4; // 4 hours max download time
        CURL_ASSERT(curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout));
        CURL_ASSERT(curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L));
        CURL_ASSERT(curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout));

        // Set url, user-agent and, headers
        CURL_ASSERT(curl_easy_setopt(curl, CURLOPT_URL, ATOMIC_CARDS_URL));
        CURL_ASSERT(curl_easy_setopt(curl, CURLOPT_USE_SSL, 1L));
        CURL_ASSERT(curl_easy_setopt(curl, CURLOPT_USERAGENT, MSE_PROJECT_NAME));
        CURL_ASSERT(curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L));
        CURL_ASSERT(curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, ""));

        // Set response write
        CURL_ASSERT(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &__mtg_json_write_callback));
        CURL_ASSERT(curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) w));

        res = curl_easy_perform(curl);

curl_set_error:
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

static void __get_all_printings_cards_curl_thread(void *data, struct thread_pool_t *pool)
{
    FILE *w = (FILE *) data;
    __do_get_all_printings_cards_curl(w);
    fclose(w);
}

int __handle_all_printings_cards_set(mtg_all_printings_cards_t *ret, const char *set_code, json_t *set_node)
{
    mtg_set_t set;
    ASSERT(parse_set_json(set_node, &set, set_code));
    free_set(&set);
    return 1;
}

int __parse_all_printings_cards(mtg_all_printings_cards_t *ret, json_t *cards)
{
    // Print debug information
    json_t *meta = json_object_get(cards, "meta");
    json_t *meta_date = json_object_get(meta, "date");
    json_t *meta_version = json_object_get(meta, "version");

    ASSERT(json_is_string(meta_date));
    ASSERT(json_is_string(meta_version));

    lprintf(LOG_INFO, "Reading cards from MTGJSON (%s) released at: %s\n",
            json_string_value(meta_date),
            json_string_value(meta_version));

    // Iterate over all of the sets
    json_t *data = json_object_get(cards, "data");
    ASSERT(json_is_object(data));

    // A nice macro in jansson <3
    const char *key;
    json_t *value;
    size_t count = 0;
    json_object_foreach(data, key, value) {
        __handle_all_printings_cards_set(ret, key, value);
        count++;
    }

    lprintf(LOG_INFO, "Found %ud sets\n", count);

    return 1;
}

int get_all_printings_cards(mtg_all_printings_cards_t *ret, thread_pool_t *pool)
{
    memset(&ret, 0, sizeof(ret));

    // Create the pipe that will be used for IPC
    int fid[2];
    int c = pipe(fid);
    ASSERT(c == 0);

    FILE *r = fdopen(fid[0], "r");
    ASSERT(r != NULL);

    FILE *w = fdopen(fid[1], "w");
    ASSERT(w != NULL);

    // Start curl request that writes to a pipe in another thread
    task_t task = {(void *) w, &__get_all_printings_cards_curl_thread};
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
    int status = __parse_all_printings_cards(ret, json);
    json_decref(json);

    if (!status) {
        lprintf(LOG_ERROR, "Cannot parse atomic json into cards and, sets\n");
        ret_code = 0;
        goto cleanup;
    }

cleanup:
    fclose(r);
    if (!ret_code) {
        lprintf(LOG_INFO, "Could not get atomic cards\n");
        free_all_printings_cards(ret);
    }
    return ret_code;
}

void free_all_printings_cards(mtg_all_printings_cards_t *cards)
{

}
