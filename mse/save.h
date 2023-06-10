#pragma once
#include "./mtg_json.h"
#include <stdio.h>

#define MSE_CARDS_FILE_NAME "cards.data"

/// Writes all cards and sets to f
int mse_write_cards(FILE *f, mse_all_printings_cards_t *cards);

/// Reads all cards and sets to f
int mse_read_cards(FILE *f, mse_all_printings_cards_t *cards);
