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
int mse_card_trie_lookup(mse_card_trie_node_t *trie, char *str, avl_tree_node_t **ret);
void free_mse_card_trie_node(mse_card_trie_node_t *node);

char *mse_filter_text(char *str);
