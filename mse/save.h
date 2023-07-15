#pragma once
#include "./mtg_json.h"
#include "./thread_pool.h"
#include <stdio.h>

#define MSE_CARDS_FILE_NAME "cards.data"

/// Writes all cards and sets to f
extern int mse_write_cards(FILE *f, mse_all_printings_cards_t *cards);

/// Reads all cards and sets to f
extern int mse_read_cards(FILE *f, mse_all_printings_cards_t *cards);

/// Gets and indexes all cards from MSE_CARDS_FILE_NAME
extern int mse_get_cards_from_file(mse_all_printings_cards_t *cards, mse_thread_pool_t *pool);
