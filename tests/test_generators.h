#pragma once
#include "../src/thread_pool.h"
#include "../src/mtg_json.h"

int test_generators();
/// Called in test_generators, defined here so that the long messy code can be in its own unit
int test_generator_validation();
int test_set_generators_double_fields();
int test_generator_set();
int test_generator_txt();
int test_generator_colours();

extern thread_pool_t gen_thread_pool;
extern mse_all_printings_cards_t gen_cards;
