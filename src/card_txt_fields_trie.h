#pragma once
#include "./card.h"
#include "./avl_tree.h"

/// We use the latin alaphabet, if you are not using this alphabet then I am sorry
/// Here is an ascii cat to make you feel better
/*
     _______           ZZZZ
   / __    \  /\/\ ZZZZ
  /    \    \/ uu \
 / _____|_  > ____/<
 \________)______))

*/
#define MSE_ALPHABET_LENGTH 26

typedef struct mse_card_trie_node_t {
    struct mse_card_trie_node_t *children[MSE_ALPHABET_LENGTH];
    avl_tree_node_t *cards;
} mse_card_trie_node_t;

int init_mse_card_trie_node(mse_card_trie_node_t **node);
void free_mse_card_trie_node(mse_card_trie_node_t *node);

/// Inserts a card into the trie, adding nodes where needed
int mse_card_trie_insert(mse_card_trie_node_t *root, mtg_card_t *card, char *str);

/// Exact lookup for a card name, i,e: goblin motivator -> gblnmtvtr then gets the tree at gblnmtvtr
/// This is useful for card loookups in deck lists whithout correction
int mse_card_trie_lookup(mse_card_trie_node_t *trie, char *str, avl_tree_node_t **ret);

/// An aproximate lookup, goes to the card name then returns the tree of that node and all children recursively,
/// this is great for auto correction or searching
int mse_card_trie_lookup_aprox(mse_card_trie_node_t *trie, char *str, avl_tree_node_t **ret);

char *mse_filter_text(char *str);
