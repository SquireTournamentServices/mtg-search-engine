#include <stdio.h>
#include <time.h>
#include <string.h>
#include "./query_parser.h"
#include "./interpretor.h"
#include "./mtg_json.h"
#include "./thread_pool.h"
#include "../testing_h/testing.h"

static int __run_query(mse_all_printings_cards_t *cards,
                       mse_thread_pool_t *pool,
                       const char *buffer)
{
    int r = 0;
    mse_interp_node_t *root = NULL;
    ASSERT(mse_parse_input_string(buffer, &root));

    mse_search_intermediate_t s_ret;
    if (!mse_resolve_interp_tree(root, &s_ret, pool, 0, cards)) {
        goto cleanup;
    }
    lprintf(LOG_INFO, "There were %lu results\n", mse_tree_size(s_ret.node));
    mse_free_search_intermediate(&s_ret);
    r = 1;

cleanup:
    mse_free_interp_node(root);
    return r;
}

int main()
{
    tzset();
    size_t t = time(NULL);
    lprintf(LOG_INFO, "Loading cards\n");

    mse_thread_pool_t pool;
    if (!mse_init_pool(&pool)) {
        lprintf(LOG_ERROR, "Cannot create thread pool\n");
        return 1;
    }

    mse_all_printings_cards_t cards;
    memset(&cards, 0, sizeof(cards));

    if (!mse_get_all_printings_cards(&cards, &pool)) {
        lprintf(LOG_ERROR, "Cannot get cards\n");
        return 1;
    }
    lprintf(LOG_INFO, "Done in %d seconds\n", time(NULL) - t);

    char buffer[1024];
    for (; printf(ANSI_YELLOW "query > " ANSI_RESET), fgets(buffer, sizeof(buffer), stdin);) {
        if (!__run_query(&cards, &pool, buffer)) {
            lprintf(LOG_ERROR, "Cannot execute query\n");
        }
    }

    mse_free_all_printings_cards(&cards);
    mse_free_pool(&pool);
}
