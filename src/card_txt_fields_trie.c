#include "./card_txt_fields_trie.h"
#include "../testing_h/testing.h"
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

static int __mse_card_trie_lookup(mse_card_trie_node_t *trie, char *str, avl_tree_node_t **ret, int i)
{
    return 1;
}

int mse_card_trie_lookup(mse_card_trie_node_t *trie, char *str, avl_tree_node_t **ret)
{
    return __mse_card_trie_lookup(trie, str, ret, 0);
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
