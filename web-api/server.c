#include "./server.h"
#include "../testing_h/testing.h"
#include <string.h>
#include <sys/param.h>
#include <jansson.h>

#define MSE_PAGE_SIZE 50

static int __mse_jsonify_card(json_t *json, mse_card_t *card)
{
    json_t *tmp;

    ASSERT(tmp = json_string(card->name));
    ASSERT(json_object_set(json, "name", tmp) == 0);

    if (card->mana_cost != NULL) {
        ASSERT(tmp = json_string(card->mana_cost));
        ASSERT(json_object_set(json, "mana_cost", tmp) == 0);
    }

    if (card->oracle_text != NULL) {
        ASSERT(tmp = json_string(card->oracle_text));
        ASSERT(json_object_set(json, "oracle_text", tmp) == 0);
    }

    ASSERT(tmp = json_array());
    ASSERT(json_object_set(json, "types", tmp) == 0);
    for (size_t i = 0; i < card->types_count; i++) {
        json_t *type_json = json_string(card->types[i]);
        ASSERT(type_json != NULL);
        if (!json_array_append(tmp, type_json) == 0) {
            lprintf(LOG_ERROR, "Cannot append to type array\n");
            json_decref(type_json);
            return 0;
        }
    }

    ASSERT(tmp = json_real(card->power));
    ASSERT(json_object_set(json, "power", tmp) == 0);

    ASSERT(tmp = json_real(card->toughness));
    ASSERT(json_object_set(json, "toughness", tmp) == 0);

    ASSERT(tmp = json_real(card->cmc));
    ASSERT(json_object_set(json, "cmc", tmp) == 0);

    ASSERT(tmp = json_integer(card->colours));
    ASSERT(json_object_set(json, "colours", tmp) == 0);

    ASSERT(tmp = json_integer(card->colour_identity));
    ASSERT(json_object_set(json, "colour_identity", tmp) == 0);

    ASSERT(tmp = json_array());
    ASSERT(json_object_set(json, "sets", tmp) == 0);
    for (size_t i = 0; i < card->set_codes_count; i++) {
        char buffer[sizeof(*card->set_codes) + 1];
        memset(buffer, 0, sizeof(buffer));
        memcpy(buffer, card->set_codes[i], sizeof(buffer) - 1);

        json_t *type_json = json_string(buffer);
        ASSERT(type_json != NULL);
        if (!json_array_append(tmp, type_json) == 0) {
            lprintf(LOG_ERROR, "Cannot append to set code array\n");
            json_decref(type_json);
            return 0;
        }
    }
    return 1;
}

static int __mse_jsonify_search_res_impl(mse_search_result_t res,
        struct mg_connection *c,
        json_t *json,
        int page_number)
{
    // Encode the response
    json_t *arr = json_array();
    ASSERT(arr);
    ASSERT(json_object_set(json, "cards", arr) == 0);

    for (size_t i = page_number * MSE_PAGE_SIZE; i < res.cards_length && i < (page_number + 1) * MSE_PAGE_SIZE; i++) {
        json_t *card = json_object();
        ASSERT(card != NULL);
        if (!__mse_jsonify_card(card, res.cards[i])) {
            lprintf(LOG_ERROR, "Cannot jsonify card\n");
            json_decref(card);
            return 0;
        }

        if (json_array_append(arr, card) != 0) {
            lprintf(LOG_ERROR, "Cannot append card\n");
            json_decref(card);
            return 0;
        }
    }

    json_t *tmp = NULL;
    ASSERT(tmp = json_integer(MSE_PAGE_SIZE));
    ASSERT(json_object_set(json, "page_size", tmp) == 0);

    ASSERT(tmp = json_integer(page_number));
    ASSERT(json_object_set(json, "page", tmp) == 0);

    // Send the response
    char *buf = json_dumps(json, 0);
    ASSERT(buf != NULL);
    mg_http_reply(c, 200, "", "%s", buf);
    free(buf);
    return 1;
}

static int __mse_jsonify_search_res(mse_search_result_t res, struct mg_connection *c, int page_number)
{
    json_t *json = json_object();
    ASSERT(json != NULL);
    ASSERT(__mse_jsonify_search_res_impl(res, c, json, page_number));
    json_decref(json);
    return 1;
}

static void __mse_serve_query(struct mg_connection *c, char *query, mse_web_server_t *server, int page_number)
{
    lprintf(LOG_INFO, "Executing query %s\n", query);

    mse_search_result_t res;
    if (!mse_search(&server->mse, &res, query)) {
        lprintf(LOG_ERROR, "Cannot execute query\n");
        mg_http_reply(c, 400, "", "400 - Invalid query");
        return;
    }

    if (!__mse_jsonify_search_res(res, c, page_number)) {
        mg_http_reply(c, 500, "", "500 - Cannot encode output");
    }
    mse_free_search_results(&res);
}

static int __mse_get_page_number(struct mg_http_message *hm)
{
    int res = 0;
    struct mg_str *header = mg_http_get_header(hm, "page");
    if (header == NULL) {
        return 0;
    }

    char buffer[10];
    memset(buffer, 0, sizeof(buffer));
    strncpy(buffer, header->ptr, MIN(sizeof(buffer), header->len));

    res = atoi(buffer);
    ASSERT(res >= 0);
    return res;
}

static void __mse_serve(struct mg_connection *c,
                        int event,
                        void *ev_data,
                        void *fn_data)
{
    mse_web_server_t *server = (mse_web_server_t *) fn_data;

    if (event == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        if(mg_http_match_uri(hm, "/")) {
            mg_http_reply(c,
                          200,
                          "",
                          "<h1>To use this API POST to ./api where the body is the query. Set the 'page' header to the page of output you want.</h1><p>%s %s</p>",
                          MSE_PROJECT_NAME,
                          MSE_PROJECT_VERSION);
        } else if (mg_http_match_uri(hm, "/github")) {
            mg_http_reply(c, 301, "",
                          "<meta http-equiv=\"refresh\" content=\"0; URL=%s\" />",
                          MSE_REPO_URL);
        } else if (mg_http_match_uri(hm, "/api")) {
            if (hm->body.len == 0) {
                mg_http_reply(c, 400, "", "400 - Empty request body");
                return;
            }

            char *query = malloc(hm->body.len + 1);
            if (query == NULL) {
                lprintf(LOG_ERROR, "Cannot alloc query string\n");
                mg_http_reply(c, 500, "", "500 - Internal server error");
                return;
            }

            strncpy(query, hm->body.ptr, hm->body.len);
            query[hm->body.len] = 0;

            int page_number = __mse_get_page_number(hm);
            __mse_serve_query(c, query, server, page_number);
            free(query);
        } else {
            mg_http_reply(c, 404, "", "404 - Page not found");
        }
    }
}

void mse_serve(mse_web_server_t *server)
{
    struct mg_mgr mgr;
    struct mg_connection *c;

    char *bind_addr = server->bind_addr;
    if (bind_addr == NULL) {
        bind_addr = "http://127.0.0.1:4365";
    }
    mg_mgr_init(&mgr);
    c = mg_http_listen(&mgr,
                       bind_addr,
                       &__mse_serve,
                       server);

    if (c == NULL) {
        lprintf(LOG_ERROR, "Unable to init mongoose.\n");
        return;
    }

    lprintf(LOG_INFO, "Listening on %s, mongoose version %s\n", bind_addr, MG_VERSION);
    while(1) mg_mgr_poll(&mgr, 10);
    mg_mgr_free(&mgr);
}
