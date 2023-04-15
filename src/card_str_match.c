#include "./card_str_match.h"
#include "../testing_h/testing.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int mse_is_regex_str(char *str)
{
    size_t len = strlen(str);
    if (len < 2) {
        return 0;
    }
    return str[0] == '/' && str[len - 1] == '/';
}

int mse_compile_regex(char *regex, regex_t *re)
{
    return regcomp(re, regex, MSE_RE_FLAGS) == 0;
}

static int __mse_re_match(char *str, regex_t *re)
{
    return regexec(re, str, 0, NULL, 0) == 0;
}

int mse_card_oracle_matches(mtg_card_t *card, regex_t *re)
{
    if (card->oracle_text == NULL) {
        return 0;
    }
    return __mse_re_match(card->oracle_text, re);
}

int mse_card_name_matches(mtg_card_t *card, regex_t *re)
{
    if (card->name == NULL) {
        return 0;
    }
    return __mse_re_match(card->name, re);
}

// Code to allow for regex matching to be done in many threads
typedef enum mse_card_match_type_t {
    MSE_MATCH_ORACLE,
    MSE_MATCH_NAME
} mse_card_match_type_t;

typedef struct mse_card_match_t {
    mse_card_match_type_t type;
    avl_tree_node_t **res;
    char *regex;
    pthread_mutex_t lock;
    sem_t sem;
    int err;
} mse_card_match_t;

typedef struct mse_card_match_worker_data_t {
    mse_card_match_t *match_data;
    avl_tree_node_t *root;
} mse_card_match_worker_data_t;

static void __mse_match_card_do_match(avl_tree_node_t *node, mse_card_match_t *match_data, regex_t *re)
{
    int matches = 0;
    switch(match_data->type) {
    case MSE_MATCH_ORACLE:
        matches = mse_card_oracle_matches((mtg_card_t *) node->payload, re);
        break;
    case MSE_MATCH_NAME:
        matches = mse_card_name_matches((mtg_card_t *) node->payload, re);
        break;
    default:
        lprintf(LOG_ERROR, "Cannot find regex match type\n");
        match_data->err = 1;
        return;
    }

    if (matches) {
        avl_tree_node_t *node_copy = shallow_copy_tree_node(node);
        if (node_copy == NULL) {
            lprintf(LOG_ERROR, "Cannot allocate tree node\n");
            match_data->err = 1;
            return;
        }

        node_copy->cmp_payload = MSE_CARD_DEFAULT_COMPARE_FUNCTION;
        node_copy->free_payload = MSE_CARD_DEFAULT_FREE_FUNCTION;

        pthread_mutex_lock(&match_data->lock);
        int r = insert_node(match_data->res, node_copy);
        pthread_mutex_unlock(&match_data->lock);

        if (!r) {
            lprintf(LOG_ERROR, "Node insert failed\n");
            match_data->err = 1;
            free_tree(node_copy);
        }
    }
}

static void __mse_match_card_node(avl_tree_node_t *node, mse_card_match_t *match_data, regex_t *re)
{
    if (node == NULL) {
        return;
    }

    __mse_match_card_do_match(node, match_data, re);
    __mse_match_card_node(node->l, match_data, re);
    __mse_match_card_node(node->r, match_data, re);
}

static void __mse_match_card_worker(void *data, thread_pool_t *pool)
{
    mse_card_match_worker_data_t *match_data = (mse_card_match_worker_data_t *) data;

    regex_t re;
    if (!mse_compile_regex(match_data->match_data->regex, &re)) {
        match_data->match_data->err = 1;
        lprintf(LOG_ERROR, "Cannot compile regex\n");
        goto cleanup;
    }

    __mse_match_card_node(match_data->root, match_data->match_data, &re);
    regfree(&re);
cleanup:
    sem_post(&match_data->match_data->sem);
    free(match_data);
}

static int __mse_match_card_worker_enqueue(mse_card_match_t *match_data,
        thread_pool_t *pool,
        avl_tree_node_t *node,
        int h,
        regex_t *re)
{
    int sum = 0;

    // base case
    if (node == NULL) {
        return 0;
    }

    if (h < 0) {
        return 0;
    }

    // Enqueue Stuff
    if (h == 0) {
        mse_card_match_worker_data_t *data = malloc(sizeof(*data));
        if (data == NULL) {
            match_data->err = 1;
            lprintf(LOG_ERROR, "Cannot allocate thread data\n");
            return 0;
        }
        memset(data, 0, sizeof(*data));

        data->match_data = match_data;
        data->root = node;

        task_t task = {data, &__mse_match_card_worker};
        if (task_queue_enqueue(&pool->queue, task)) {
            sum++;
        } else {
            lprintf(LOG_ERROR, "Cannot enqueue regex match\n");
            match_data->err = 1;
            free(data);
            return 0;
        }
    } else {
        __mse_match_card_do_match(node, match_data, re);
    }

    // Recurse
    sum += __mse_match_card_worker_enqueue(match_data, pool, node->l, h - 1, re);
    sum += __mse_match_card_worker_enqueue(match_data, pool, node->r, h - 1, re);
    return sum;
}

static int __mse_match_cards(avl_tree_node_t **ret,
                             avl_tree_node_t *cards_tree,
                             char *regex,
                             thread_pool_t *pool,
                             mse_card_match_type_t type)
{
    regex_t re;
    ASSERT(mse_compile_regex(regex, &re));

    mse_card_match_t data;
    memset(&data, 0, sizeof(data));
    data.type = type;
    data.res = ret;
    data.regex = regex;

    pthread_mutex_t lock_tmp = PTHREAD_MUTEX_INITIALIZER;
    data.lock = lock_tmp;

    int layers = (int) floor(log2(pool->threads_count));
    sem_init(&data.sem, 0, 0); // Each thread calls up()

    int thread_cnt = __mse_match_card_worker_enqueue(&data, pool, cards_tree, layers, &re);

    // Wait for the threads then cleanup
    for (int i = 0; i < thread_cnt; i++) {
        int waiting = 1;
        while (waiting) {
            pool_try_consume(pool);
            waiting = sem_trywait(&data.sem) != 0;
        }
    }
    regfree(&re);
    pthread_mutex_destroy(&data.lock);

    ASSERT(data.err == 0);
    return 1;
}

int mse_matching_card_oracle(avl_tree_node_t **ret,
                             avl_tree_node_t *cards_tree,
                             char *regex,
                             thread_pool_t *pool)
{
    return __mse_match_cards(ret, cards_tree, regex, pool, MSE_MATCH_ORACLE);
}

int mse_matching_card_name(avl_tree_node_t **ret,
                           avl_tree_node_t *cards_tree,
                           char *regex,
                           thread_pool_t *pool)
{
    return __mse_match_cards(ret, cards_tree, regex, pool, MSE_MATCH_NAME);
}

char *escape_regex(char *regex)
{
    size_t len = strlen(regex);
    char *ret = malloc(len + 1);
    ASSERT(ret != NULL);

    size_t j = 0;
    int escaping = 0;
    for (size_t i = 0; i < len; i++) {
        // Strip the regex quote marks
        if (i == 0 && regex[i] == '/') continue;
        if (i == len - 1 && regex[i] == '/') continue;

        if (regex[i] == '\\' && !escaping) {
            escaping = 1;
        } else {
            ret[j] = regex[i];
            j++;
        }
    }
    ret[j] = 0;

    return ret;
}
