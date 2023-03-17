#include "./mtg_json_indexes.h"
#include "../testing_h/testing.h"
#include <string.h>

// Set cards index
static int __add_card_to_set(mtg_card_t *card, avl_tree_node *sets)
{
    for (size_t i = 0; i < card->set_codes_count; i++) {
        // Create a proxy element for the tree search as the tree will be comparing mtg_set_t objects
        mtg_set_t card_set_proxy;
        memcpy(card_set_proxy.code, card->set_codes[i], sizeof(card_set_proxy.code));

        avl_tree_node *set_node = find_payload(sets, &card_set_proxy);

        if (set_node != NULL) {
            add_card_to_set((mtg_set_t *) set_node->payload, card);
        } else {
            char set_code[MAX_SET_CODE_LEN + 1];
            set_code[MAX_SET_CODE_LEN] = 0;
            memcpy(set_code, card->set_codes[i], sizeof(card->set_codes[i]));

            lprintf(LOG_ERROR, "Cannot find set with code %s for card %s\n", set_code, card->name);
            return 0;
        }
    }
    return 1;
}

static int __add_cards_to_set(avl_tree_node *cards, avl_tree_node *sets)
{
    if (cards == NULL) {
        return 1;
    }

    ASSERT(__add_card_to_set((mtg_card_t *) cards->payload, sets));
    ASSERT(__add_cards_to_set(cards->l, sets));
    ASSERT(__add_cards_to_set(cards->r, sets));
    return 1;
}

static void __generate_set_cards_index_task(void *__state, thread_pool_t *pool)
{
    mse_index_generator_state_t *state = (mse_index_generator_state_t *) __state;
    if (!__add_cards_to_set(state->cards->card_tree, state->cards->set_tree)) {
        state->ret = 0;
    }
    sem_post(&(state->semaphore));
}

static int __insert_node(avl_tree_node **root, avl_tree_node *node)
{
    if (*root == NULL) {
        *root = node;
        return 1;
    } else {
        return insert_node(root, node);
    }
}

// Power index
static int __add_cards_to_p_tree(avl_tree_node *cards, avl_tree_node **card_p_tree)
{
    if (cards == NULL) {
        return 1;
    }

    avl_tree_node *node = init_avl_tree_node(NULL, &avl_cmp_card_p, cards->payload);
    int r = __insert_node(card_p_tree, node);
    if (!r) {
        lprintf(LOG_ERROR, "Cannot insert a card into the power tree\n");
        free_tree(node);
        return 0;
    }
    ASSERT(__add_cards_to_p_tree(cards->l, card_p_tree));
    ASSERT(__add_cards_to_p_tree(cards->r, card_p_tree));
    return 1;
}

static void __generate_card_p_index_task(void *__state, thread_pool_t *pool)
{
    mse_index_generator_state_t *state = (mse_index_generator_state_t *) __state;
    if (!__add_cards_to_p_tree(state->cards->card_tree, &state->cards->indexes.card_p_tree)) {
        state->ret = 0;
    }
    sem_post(&(state->semaphore));
}

// Toughness index
static int __add_cards_to_t_tree(avl_tree_node *cards, avl_tree_node **card_t_tree)
{
    if (cards == NULL) {
        return 1;
    }

    avl_tree_node *node = init_avl_tree_node(NULL, &avl_cmp_card_t, cards->payload);
    int r = __insert_node(card_t_tree, node);
    if (!r) {
        lprintf(LOG_ERROR, "Cannot insert a card into the power tree\n");
        free_tree(node);
        return 0;
    }
    ASSERT(__add_cards_to_t_tree(cards->l, card_t_tree));
    ASSERT(__add_cards_to_t_tree(cards->r, card_t_tree));
    return 1;
}

static void __generate_card_t_index_task(void *__state, thread_pool_t *pool)
{
    mse_index_generator_state_t *state = (mse_index_generator_state_t *) __state;
    if (!__add_cards_to_t_tree(state->cards->card_tree, &state->cards->indexes.card_t_tree)) {
        state->ret = 0;
    }
    sem_post(&(state->semaphore));
}


#define TASK_COUNT(T) (sizeof(T) / sizeof(*T))

int __generate_indexes(mtg_all_printings_cards_t *ret, thread_pool_t *pool)
{
    ASSERT(pool != NULL);
    ASSERT(ret != NULL);

    void (*tasks[])(void *, struct thread_pool_t *) = {&__generate_set_cards_index_task,
                                                       &__generate_card_p_index_task,
                                                       &__generate_card_t_index_task
                                                      };

    mse_index_generator_state_t state;
    state.ret = 1;
    state.cards = ret;
    state.pool = pool;
    sem_init(&state.semaphore, 0, 0);

    // Start the tasks
    size_t len = TASK_COUNT(tasks);
    lprintf(LOG_INFO, "Generating %lu indexes\n", len);
    for (size_t i = 0; i < len; i++) {
        task_t task = {(void *) &state, tasks[i]};
        ASSERT(task_queue_enqueue(&pool->queue, task));
    }

    // Wait for all of the tasks to complete on the thread pool
    for (size_t i = 0; i < TASK_COUNT(tasks); i++) {
        sem_wait(&state.semaphore);
    }
    int tmp = state.ret;
    free_index_generator_state(&state);

    ASSERT(tmp);
    return 1;
}

void free_index_generator_state(mse_index_generator_state_t *state)
{
    sem_destroy(&state->semaphore);
    memset(state, 0, sizeof(*state));
}