#pragma once
#include <time.h>
#include <stdio.h>
#include <jansson.h>
#include "./uuid.h"
#include "./avl_tree.h"

typedef struct mtg_set_t {
    char *name;
    mtg_set_code_t code;
    struct tm release;
    /// The payload is a pointer to a card, the card is allocated by the cards, therefore there is no free function set.
    avl_tree_node *set_cards;
} mtg_set_t;

int parse_set_json(json_t *set_node, mtg_set_t *ret, const char *code);
int write_set(FILE *f, mtg_set_t set);
int read_set(FILE *f, mtg_set_t *set);
void free_set(mtg_set_t *set);

int cmp_set(mtg_set_t *a, mtg_set_t *b);
int avl_cmp_set(void *a, void *b);
