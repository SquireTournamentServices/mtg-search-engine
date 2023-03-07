#pragma once
#include "./thread_pool.h"
#include "./card.h"
#include "./set.h"

#define MSE_GZ_SUFFIX ".gz"
#define ATOMIC_CARDS_URL "https://mtgjson.com/api/v5/AtomicCards.json"

/// This will store the atomic cards and, sets. Each card is stored once and,
/// only once. cards and, sets are both stored as binary trees sorted by UUIDs.
/// other indices need to be generated afterwards.
typedef struct mtg_atomic_cards_t {
    size_t cards_len;
    mtg_card_t *cards;
    size_t sets_len;
    mtg_set_t *sets;
} mtg_atomic_cards_t;

/// Returns 1 on success, 0 on failure
int get_atomic_cards(mtg_atomic_cards_t *ret, thread_pool_t *pool);
void free_atomic_cards(mtg_atomic_cards_t *cards);

/// This will store all cards  and, their indexes
typedef struct mtg_indexed_cards_t {
    mtg_atomic_cards_t atomic_cards;

} mtg_indexed_cards_t;

