#pragma once
#include <time.h>
#include <stdio.h>
#include <jansson.h>
#include "./uuid.h"
#include "./avl_tree.h"
#include "./card.h"

typedef struct mse_set_t {
    char *name;
    mse_set_code_t code;
    struct tm release;
    /// The payload is a pointer to a card, the card is allocated by the cards, therefore there is no free function set.
    mse_avl_tree_node_t *set_cards_tree;
} mse_set_t;

int mse_parse_set_json(json_t *set_node, mse_set_t *ret, const char *code);
int mse_write_set(FILE *f, mse_set_t set);
int mse_read_set(FILE *f, mse_set_t *set);
int mse_add_card_to_set(mse_set_t *set, mse_card_t *card);
void mse_free_set(mse_set_t *set);

int mse_cmp_set(mse_set_t *a, mse_set_t *b);
int mse_avl_cmp_set(void *a, void *b);
