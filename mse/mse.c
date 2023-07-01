#include "./mse.h"
#include "./query_parser.h"
#include "./interpretor.h"
#include "./save.h"
#include "../testing_h/testing.h"
#include <string.h>
#include <time.h>

static int __mse_get_and_save_cards(mse_t *state)
{
    if (!mse_get_all_printings_cards(&state->cards, &state->pool)) {
        lprintf(LOG_ERROR, "Cannot get cards\n");
        return 1;
    }

    lprintf(LOG_INFO, "Saving cards...\n");
    FILE *f = fopen(MSE_CARDS_FILE_NAME, "wb");
    if (f == NULL) {
        lprintf(LOG_ERROR, "Cannot open file\n");
        return 0;
    }

    if (!mse_write_cards(f, &state->cards)) {
        lprintf(LOG_ERROR, "Cannot save cards\n");
    }
    fclose(f);
    return 1;
}

int mse_init(mse_t *state)
{
    memset(state, 0, sizeof(*state));
    tzset();

    size_t t = time(NULL);
    ASSERT(mse_init_pool(&state->pool));

    lprintf(LOG_INFO, "Loading cards\n");
    if (!mse_get_cards_from_file(&state->cards, &state->pool)) {
        if (!__mse_get_and_save_cards(state)) {
            lprintf(LOG_WARNING, "Cannot save cards to the file system.\n");
        }
    }

    lprintf(LOG_INFO, "Done in %d seconds\n", time(NULL) - t);
    return 1;
}

static int __mse_search(mse_t *state, mse_search_intermediate_t *ret, const char *query)
{
    mse_interp_node_t *root = NULL;
    ASSERT(mse_parse_input_string(query, &root));

    if (!mse_resolve_interp_tree(root, ret, &state->pool, 0, &state->cards)) {
        mse_free_interp_node(root);
        mse_free_search_intermediate(ret);
        return 0;
    }
    mse_free_interp_node(root);
    lprintf(LOG_INFO, "There were %lu results\n", mse_tree_size(ret->node));
    return 1;
}

int mse_search(mse_t *state, mse_search_result_t *res, const char *query)
{
    mse_search_intermediate_t tmp;
    ASSERT(__mse_search(state, &tmp, query));
    ASSERT(mse_finalise_search(res, &tmp));
    return 1;
}

void mse_free(mse_t *state)
{
    mse_free_all_printings_cards(&state->cards);
    mse_free_pool(&state->pool);
    memset(state, 0, sizeof(*state));
}
