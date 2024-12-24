#include "./save.h"
#include "./io_utils.h"
#include "../testing_h/testing.h"
#include <string.h>

static int __mse_write_cards_set(FILE *f, mse_avl_tree_node_t *node)
{
    if (node == NULL) {
        return 1;
    }

    mse_set_t *set = (mse_set_t *) node->payload;
    ASSERT(mse_write_set(f, *set));

    ASSERT(__mse_write_cards_set(f, node->l));
    ASSERT(__mse_write_cards_set(f, node->r));
    return 1;
}

static int __mse_write_cards_sets(FILE *f, mse_all_printings_cards_t *cards)
{
    size_t len = mse_tree_size(cards->set_tree);
    ASSERT(mse_write_size_t(f, len));
    ASSERT(__mse_write_cards_set(f, cards->set_tree));
    return 1;
}

static int __mse_write_cards_card(FILE *f, mse_avl_tree_node_t *node)
{
    if (node == NULL) {
        return 1;
    }

    mse_card_t *card = (mse_card_t *) node->payload;
    ASSERT(mse_write_card(f, *card));

    ASSERT(__mse_write_cards_card(f, node->l));
    ASSERT(__mse_write_cards_card(f, node->r));
    return 1;
}

static int __mse_write_cards_cards(FILE *f, mse_all_printings_cards_t *cards)
{
    size_t len = mse_tree_size(cards->card_tree);
    ASSERT(mse_write_size_t(f, len));
    ASSERT(__mse_write_cards_card(f, cards->card_tree));
    return 1;
}

int mse_write_cards(FILE *f, mse_all_printings_cards_t *cards)
{
    ASSERT(mse_write_size_t(f, MSE_BINARY_VERSION));
    ASSERT(__mse_write_cards_sets(f, cards));
    ASSERT(__mse_write_cards_cards(f, cards));
    return 1;
}

static void __free_save_cards_card(void *card)
{
    mse_free_card((mse_card_t *) card);
    free(card);
}

static void __free_save_cards_set(void *set)
{
    mse_free_set((mse_set_t *) set);
    free(set);
}

static int __mse_read_cards_sets(FILE *f, mse_all_printings_cards_t *cards)
{
    size_t len;
    ASSERT(mse_read_size_t(f, &len));

    for (size_t i = 0; i < len; i++) {
        mse_set_t *set = malloc(sizeof(*set));
        ASSERT(set != NULL);
        ASSERT(mse_read_set(f, set));

        mse_avl_tree_node_t *node = mse_init_avl_tree_node(&__free_save_cards_set, &mse_avl_cmp_set, set);
        ASSERT(mse_insert_node(&cards->set_tree, node));
        cards->set_count++;
    }
    return 1;
}

static int __mse_read_cards_cards(FILE *f, mse_all_printings_cards_t *cards)
{
    size_t len;
    ASSERT(mse_read_size_t(f, &len));

    for (size_t i = 0; i < len; i++) {
        mse_card_t *card = malloc(sizeof(*card));
        ASSERT(card != NULL);
        mse_avl_tree_node_t *node = mse_init_avl_tree_node(&__free_save_cards_card, &mse_avl_cmp_card, card);

        ASSERT(mse_read_card(f, card));
        if (mse_insert_node(&cards->card_tree, node)) {
            cards->card_count++;
        } else {
            mse_free_tree(node);
        }
    }
    return 1;
}

int mse_read_cards(FILE *f, mse_all_printings_cards_t *cards)
{
    memset(cards, 0, sizeof(*cards));
    size_t bin_version;
    ASSERT(mse_read_size_t(f, &bin_version));

    if (bin_version != MSE_BINARY_VERSION) {
        lprintf(LOG_ERROR, "Version mismatch! Data file is version %lu, expected version %lu\n", bin_version, MSE_BINARY_VERSION);
        return 0;
    }

    ASSERT(__mse_read_cards_sets(f, cards));
    ASSERT(__mse_read_cards_cards(f, cards));
    return 1;
}

int __mse_get_cards_from_file(FILE *f, mse_all_printings_cards_t *cards, mse_thread_pool_t *pool)
{
    ASSERT(mse_read_cards(f, cards));
    lprintf(LOG_INFO, "Found %lu sets and, %lu cards\n", cards->set_count, cards->card_count);

    lprintf(LOG_INFO, "Generating card indexes\n");
    ASSERT(__mse_generate_indexes(cards, pool));

    lprintf(LOG_INFO, "Cards and, indexes are now complete\n");
    return 1;
}

int mse_get_cards_from_file(mse_all_printings_cards_t *cards, mse_thread_pool_t *pool)
{
    lprintf(LOG_INFO, "Reading cards from %s, expecting version %lu\n", MSE_CARDS_FILE_NAME, MSE_BINARY_VERSION);

    FILE *f = fopen(MSE_CARDS_FILE_NAME, "rb");
    ASSERT(f != NULL);

    int r = __mse_get_cards_from_file(f, cards, pool);
    fclose(f);
    ASSERT(r);
    return 1;
}
