#include "./mtg_json_indexes.h"
#include "../testing_h/testing.h"
#include <string.h>

static void __add_card_to_set(mtg_card_t *card, avl_tree_node *sets)
{
    for (size_t i = 0; i < card->set_codes_count; i++) {
        avl_tree_node *set_node = find_payload(sets, card->set_codes[i]);
        add_card_to_set((mtg_set_t *) set_node->payload, card);
    }
}

static void __add_cards_to_set(avl_tree_node *cards, avl_tree_node *sets)
{
    if (cards == NULL) {
        return;
    }

    __add_card_to_set((mtg_card_t *) cards->payload, sets);
    __add_cards_to_set(cards->l, sets);
    __add_cards_to_set(cards->r, sets);
}

static void __generate_set_cards_index_task(void *__state, thread_pool_t *pool)
{
    mse_index_generator_state_t *state = (mse_index_generator_state_t *) __state;
    __add_cards_to_set(state->cards->card_tree, state->cards->set_tree);
    sem_post(&(state->semaphore));
}

#define TASK_COUNT(T) (sizeof(T) / sizeof(*T))

int __generate_indexes(mtg_all_printings_cards_t *ret, thread_pool_t *pool)
{
    ASSERT(pool != NULL);
    ASSERT(ret != NULL);

    void (*tasks[])(void *, struct thread_pool_t *) = {&__generate_set_cards_index_task};

    mse_index_generator_state_t state;
    sem_init(&state.semaphore, 0, 0);

    // Wait for all of the tasks to complete on the thread pool
    for (size_t i = 0; i < TASK_COUNT(tasks); i++) {
        sem_wait(&state.semaphore);
    }
    free_index_generator_state(&state);
    return 1;
}

void free_index_generator_state(mse_index_generator_state_t *state)
{
    sem_destroy(&state->semaphore);
    memset(state, 0, sizeof(*state));
}
