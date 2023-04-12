#include "./mtg_json_indexes.h"
#include "../testing_h/testing.h"
#include <string.h>

// Set cards index
static int __add_card_to_set(mtg_card_t *card, avl_tree_node_t *sets)
{
    for (size_t i = 0; i < card->set_codes_count; i++) {
        // Create a proxy element for the tree search as the tree will be comparing mtg_set_t objects
        mtg_set_t card_set_proxy;
        memcpy(card_set_proxy.code, card->set_codes[i], sizeof(card_set_proxy.code));

        avl_tree_node_t *set_node = find_payload(sets, &card_set_proxy);

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

static int __add_cards_to_set(avl_tree_node_t *cards, avl_tree_node_t *sets)
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

static int __insert_node(avl_tree_node_t **root, avl_tree_node_t *node)
{
    if (*root == NULL) {
        *root = node;
        return 1;
    } else {
        return insert_node(root, node);
    }
}

#define MSE_INDEX_FIELD_NAME(fname) __generate_card_##fname##_index_task
#define MSE_INDEX_FOR_FIELD(fname) \
static int __add_cards_to_##fname##_tree(avl_tree_node_t *cards, avl_tree_node_t **card_##fname##_tree) \
{ \
    if (cards == NULL) { \
        return 1; \
    } \
 \
    avl_tree_node_t *node = init_avl_tree_node(NULL, &avl_cmp_card_##fname, cards->payload); \
    int r = __insert_node(card_##fname##_tree, node); \
    if (!r) { \
        lprintf(LOG_ERROR, "Cannot insert a card into the " #fname " tree\n"); \
        free_tree(node); \
        return 0; \
    } \
    ASSERT(__add_cards_to_##fname##_tree(cards->l, card_##fname##_tree)); \
    ASSERT(__add_cards_to_##fname##_tree(cards->r, card_##fname##_tree)); \
    return 1; \
} \
static void MSE_INDEX_FIELD_NAME(fname)(void *__state, thread_pool_t *pool) \
{ \
    mse_index_generator_state_t *state = (mse_index_generator_state_t *) __state; \
    if (!__add_cards_to_##fname##_tree(state->cards->card_tree,  \
                                      &state->cards->indexes.card_##fname##_tree)) { \
        state->ret = 0; \
    } \
    sem_post(&(state->semaphore)); \
}

MSE_INDEX_FOR_FIELD(power);
MSE_INDEX_FOR_FIELD(toughness);
MSE_INDEX_FOR_FIELD(cmc);

static int __add_cards_to_card_name_trie(avl_tree_node_t *node, mse_card_trie_node_t *card_name_trie)
{
    if (node == NULL) {
        return 1;
    }

    mtg_card_t *card = (mtg_card_t *) node->payload;
    ASSERT(mse_card_trie_insert(card_name_trie, card, card->name));

    ASSERT(__add_cards_to_card_name_trie(node->l, card_name_trie));
    ASSERT(__add_cards_to_card_name_trie(node->r, card_name_trie));
    return 1;
}

static void __generate_card_name_trie_task(void *__state, thread_pool_t *pool)
{
    mse_index_generator_state_t *state = (mse_index_generator_state_t *) __state;
    if (!__add_cards_to_card_name_trie(state->cards->card_tree, state->cards->indexes.card_name_trie)) {
        state->ret = 0;
    }
    sem_post(&(state->semaphore));
}

#define TASK_COUNT(T) (sizeof(T) / sizeof(*T))

int __generate_indexes(mtg_all_printings_cards_t *ret, thread_pool_t *pool)
{
    ASSERT(pool != NULL);
    ASSERT(ret != NULL);
    ASSERT(init_mse_card_trie_node(&ret->indexes.card_name_trie));

    void (*tasks[])(void *, struct thread_pool_t *) = {&__generate_set_cards_index_task,
                                                       &__generate_card_name_trie_task,
                                                       &MSE_INDEX_FIELD_NAME(power),
                                                       &MSE_INDEX_FIELD_NAME(toughness),
                                                       &MSE_INDEX_FIELD_NAME(cmc)
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
