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
    avl_tree_node_t *set_cards_tree;
} mse_set_t;

int parse_set_json(json_t *set_node, mse_set_t *ret, const char *code);
int write_set(FILE *f, mse_set_t set);
int read_set(FILE *f, mse_set_t *set);
int add_card_to_set(mse_set_t *set, mse_card_t *card);
void free_set(mse_set_t *set);

int cmp_set(mse_set_t *a, mse_set_t *b);
int avl_cmp_set(void *a, void *b);
