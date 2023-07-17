#include "./set.h"
#include "./io_utils.h"
#include "../testing_h/testing.h"
#include "../strptime/strptime.h"
#include <string.h>
#include <time.h>

// Parse dates in the form 2007-07-13, see man strptime.h
#define SET_DATE_FORMAT "%Y-%m-%d"

int mse_parse_set_json(json_t *set_node, mse_set_t *ret, const char *code)
{
    memset(ret, 0, sizeof(*ret));
    ASSERT(set_node != NULL);
    ASSERT(ret != NULL);

    // Copy set code
    ASSERT(mse_get_set_code(code, &ret->code));

    // Copy set name
    json_t *name_node = json_object_get(set_node, "name");
    ASSERT(json_is_string(name_node));
    ASSERT(ret->name = strdup(json_string_value(name_node)));

    // Copy set release date
    json_t *release_node = json_object_get(set_node, "releaseDate");
    ASSERT(json_is_string(release_node));
    ASSERT(strptime(json_string_value(release_node), SET_DATE_FORMAT, &ret->release) != NULL);

    return 1;
}

int mse_write_set(FILE *f, mse_set_t set)
{
    ASSERT(mse_write_str(f, set.name));
    ASSERT(mse_write_set_code(f, set.code));
    ASSERT(mse_write_tm(f, set.release));
    return 1;
}

int mse_read_set(FILE *f, mse_set_t *set)
{
    memset(set, 0, sizeof(*set));
    ASSERT(mse_read_str(f, &set->name));
    ASSERT(mse_read_set_code(f, &set->code));
    ASSERT(mse_read_tm(f, &set->release));
    return 1;
}

int mse_add_card_to_set(mse_set_t *set, mse_card_t *card)
{
    ASSERT(set != NULL);
    ASSERT(card != NULL);

    // The card's memory is owned by the index struct that the set struct is part of
    mse_avl_tree_node_t *node = mse_init_avl_tree_node(NULL, &mse_avl_cmp_card, (void *) card, set->parent);
    if (node->region_ptr == NULL && node->region_length == 1) {
        set->parent = node;
    }

    ASSERT(node != NULL);

    if (!mse_insert_node(&set->set_cards_tree, node)) {
        mse_free_tree(node);
        return 0;
    }
    return 1;
}

void mse_free_set(mse_set_t *set)
{
    if (set->name != NULL) {
        free(set->name);
    }

    if (set->set_cards_tree != NULL) {
        mse_free_tree(set->set_cards_tree);
    }
    memset(set, 0, sizeof(*set));
}

int mse_cmp_set(mse_set_t *a, mse_set_t *b)
{
    return memcmp(a->code, b->code, sizeof(a->code));
}

int mse_avl_cmp_set(void *a, void *b)
{
    return mse_cmp_set((mse_set_t *) a, (mse_set_t *) b);
}
