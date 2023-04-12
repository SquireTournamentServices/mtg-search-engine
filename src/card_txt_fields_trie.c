#include "./card_txt_fields_trie.h"
#include "../testing_h/testing.h"
#include "./search.h"
#include "mse_char_map.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int init_mse_card_trie_node(mse_card_trie_node_t **node)
{
    *node = malloc(sizeof(**node));
    ASSERT(node != NULL);

    memset(*node, 0, sizeof(**node));
    return 1;
}

void free_mse_card_trie_node(mse_card_trie_node_t *node)
{
    if (node == NULL) {
        return;
    }

    for (size_t i = 0; i < MSE_ALPHABET_LENGTH; i++) {
        free_mse_card_trie_node(node->children[i]);
    }

    if (node->cards != NULL) {
        free_tree(node->cards);
    }
    free(node);
}

static int __mse_card_trie_lookup(mse_card_trie_node_t *root, char *str, avl_tree_node_t **ret, int i)
{
    if (str[i] == 0) {
        // Copy the tree
        mse_search_intermediate_t r, a, b;
        memset(&r, 0, sizeof(r));
        b = r;
        a.node = root->cards;

        ASSERT(mse_set_union(&r, &a, &b));
        *ret = r.node;
        return 1;
    }

    long c_index = str[i] - 'a';

    // Sanity checks
    ASSERT(c_index >= 0);
    ASSERT(c_index < (long) (sizeof(root->children) / sizeof(*root->children)));

    // Fails to find
    if (root->children[c_index] == NULL) {
        return 0;
    }
    return __mse_card_trie_lookup(root->children[c_index], str, ret, i + 1);
}

int mse_card_trie_lookup(mse_card_trie_node_t *trie, char *str, avl_tree_node_t **ret)
{
    *ret = NULL;
    char *str_f = mse_filter_text(str);
    ASSERT(str_f != NULL);
    int r = __mse_card_trie_lookup(trie, str_f, ret, 0);

    free(str_f);
    return r;
}

static int __mse_insert_avl(avl_tree_node_t **root, avl_tree_node_t *node)
{
    if (node == NULL) {
        return 1;
    }

    avl_tree_node_t *node_copy = shallow_copy_tree_node(node);
    ASSERT(node_copy != NULL);

    node_copy->cmp_payload = MSE_CARD_DEFAULT_COMPARE_FUNCTION;
    node_copy->free_payload = MSE_CARD_DEFAULT_FREE_FUNCTION;

    int r = insert_node(root, node_copy);
    if (!r) {
        free_tree(node_copy);
    }

    ASSERT(__mse_insert_avl(root, node->l));
    ASSERT(__mse_insert_avl(root, node->r));
    return 1;
}

static int __mse_insert_trie_children(mse_card_trie_node_t *node, avl_tree_node_t **ret)
{
    ASSERT(__mse_insert_avl(ret, node->cards));
    for (size_t i  = 0; i < sizeof(node->children) / sizeof(*node->children); i++) {
        __mse_insert_trie_children(node->children[i], ret);
    }
    return 1;
}

static int __mse_card_trie_lookup_aprox(mse_card_trie_node_t *root, char *str, avl_tree_node_t **ret, int i)
{
    if (str[i] == 0) {
        ASSERT(__mse_insert_trie_children(root, ret));
        return 1;
    }

    long c_index = str[i] - 'a';

    // Sanity checks
    ASSERT(c_index >= 0);
    ASSERT(c_index < (long) (sizeof(root->children) / sizeof(*root->children)));

    // Fails to find
    if (root->children[c_index] == NULL) {
        return 0;
    }
    return __mse_card_trie_lookup(root->children[c_index], str, ret, i + 1);
}

int mse_card_trie_lookup_aprox(mse_card_trie_node_t *trie, char *str, avl_tree_node_t **ret)
{
    *ret = NULL;
    char *str_f = mse_filter_text(str);
    ASSERT(str_f != NULL);
    int r = __mse_card_trie_lookup_aprox(trie, str_f, ret, 0);

    free(str_f);
    return r;
}

static int __mse_card_trie_do_insert(mse_card_trie_node_t *root, mtg_card_t *card)
{
    avl_tree_node_t *node = init_avl_tree_node(MSE_CARD_DEFAULT_FREE_FUNCTION,
                            MSE_CARD_DEFAULT_COMPARE_FUNCTION,
                            (void *) card);
    ASSERT(node != NULL);
    ASSERT(insert_node(&root->cards, node));
    return 1;
}

static int __mse_card_trie_insert(mse_card_trie_node_t *root, mtg_card_t *card, char *str, int index)
{
    if (str[index] == 0) {
        return __mse_card_trie_do_insert(root, card);
    }

    long c_index = str[index] - 'a';

    // Sanity checks
    ASSERT(c_index >= 0);
    ASSERT(c_index < (long) (sizeof(root->children) / sizeof(*root->children)));

    // Insert the trie node if needed
    if (root->children[c_index] == NULL) {
        ASSERT(init_mse_card_trie_node(&root->children[c_index]));
    }
    return __mse_card_trie_insert(root->children[c_index], card, str, index + 1);
}

int mse_card_trie_insert(mse_card_trie_node_t *root, mtg_card_t *card, char *str)
{
    char *str_f = mse_filter_text(str);
    ASSERT(str_f != NULL);

    int r = __mse_card_trie_insert(root, card, str_f, 0);
    free(str_f);

    ASSERT(r);
    return 1;
}

#define MSE_FILTER_NO_CHAR 0

char __mse_filter_char(char c)
{
    // Unsigns your int, declines to elabourate
    size_t i = (size_t) *(unsigned char *)((void *) &c);
    return mse_filter_char_map[i];
}

static int __is_duplicate(char *str, int i, char c)
{
    if (i > 0) {
        return str[i - 1] == c;
    }
    return 0;
}

char *mse_filter_text(char *str)
{
    size_t len = strlen(str);
    char *ret = malloc(len + 1);
    ASSERT(ret != NULL);

    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        char c = __mse_filter_char(str[i]);
        if (c == MSE_FILTER_NO_CHAR) {
            continue;
        }
        if (__is_duplicate(ret, j, c)) {
            continue;
        }

        ret[j] = c;
        j++;
    }
    ret[j] = 0;

    return ret;
}
