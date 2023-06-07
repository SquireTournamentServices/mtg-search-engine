#pragma once
#include "./card.h"
#include "./avl_tree.h"
#include "mse_char_map.h"

typedef struct mse_card_trie_node_t {
    struct mse_card_trie_node_t *children[MSE_ALPHABET_LENGTH];
    mse_avl_tree_node_t *cards;
} mse_card_trie_node_t;

int mse_init_card_trie_node(mse_card_trie_node_t **node);
void mse_free_card_trie_node(mse_card_trie_node_t *node);

/// Inserts a card into the trie, adding nodes where needed
int mse_card_trie_insert(mse_card_trie_node_t *root, mse_card_t *card, char *str);

/// Exact lookup for a card name, i,e: goblin motivator -> gblnmtvtr then gets the tree at gblnmtvtr
/// This is useful for card loookups in deck lists whithout correction
int mse_card_trie_lookup(mse_card_trie_node_t *trie, char *str, mse_avl_tree_node_t **ret);

/// An aproximate lookup, goes to the card name then returns the tree of that node and all children recursively,
/// this is great for auto correction or searching
int mse_card_trie_lookup_aprox(mse_card_trie_node_t *trie, char *str, mse_avl_tree_node_t **ret);

char *mse_filter_text(char *str);

/// This type is the wrapper for the parts of a card name, i.e:
/// Goblin Motivator -> {gbln, mtvtr}
/// These parts of the name are also filtered to make them able to be passed into a trie straight away
typedef struct mse_card_name_parts_t {
    char **parts;
    size_t len;
} mse_card_name_parts_t;

int mse_split_card_name(char *name, mse_card_name_parts_t *ret);
void mse_free_card_parts(mse_card_name_parts_t *ret);
