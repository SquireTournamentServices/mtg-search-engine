#pragma once
#include "../src/thread_pool.h"
#include "../src/mtg_json.h"

int test_generators();
/// Called in test_generators, defined here so that the long messy code can be in its own unit
int test_generator_validation();
int test_set_generators();

extern thread_pool_t gen_thread_pool;
extern mtg_all_printings_cards_t gen_cards;
