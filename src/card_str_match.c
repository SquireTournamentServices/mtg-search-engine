#include "./card_str_match.h"
#include "./io_utils.h"
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

#define __MSE_HASH_DEFAULT 0

int mse_str_match(char *str, char *substr)
{
    size_t substr_len = strlen(substr);
    long w_hash = __MSE_HASH_DEFAULT; // Window hash

    // Calculate target hash
    long substr_hash = __MSE_HASH_DEFAULT;
    for (size_t i = 0; i < substr_len; i++) {
        substr_hash += substr[i];
    }

    for (size_t i = 0; str[i] != 0; i++) {
        // Slide the window hash along
        w_hash += str[i];
        if (i >= substr_len) {
            w_hash -= str[i - substr_len];
        }

        // Check that a hash match is not a false positive
        if (w_hash == substr_hash && i + 1 >= substr_len) {
            if (memcmp(&str[i - substr_len + 1], substr, substr_len) == 0) {
                return 1;
            }
        }
    }
    return 0;
}

int mse_card_oracle_matches(mse_card_t *card, regex_t *re)
{
    if (card->oracle_text == NULL) {
        return 0;
    }
    return __mse_re_match(card->oracle_text, re);
}

int mse_card_name_matches(mse_card_t *card, regex_t *re)
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
    int is_regex;
} mse_card_match_t;

typedef struct mse_card_match_cmp_data_t {
    union {
        regex_t *re;
        char *substr;
    };
} mse_card_match_cmp_data_t;

typedef struct mse_card_match_worker_data_t {
    mse_card_match_t *match_data;
    avl_tree_node_t *root;
} mse_card_match_worker_data_t;

static void __mse_match_card_do_match(avl_tree_node_t *node, mse_card_match_t *match_data, mse_card_match_cmp_data_t data)
{
    int matches = 0;
    switch(match_data->type) {
    case MSE_MATCH_ORACLE:
        if (match_data->is_regex) {
            matches = mse_card_oracle_matches((mse_card_t *) node->payload, data.re);
        } else {
            matches = mse_str_match(((mse_card_t *) node->payload)->oracle_text_lower, data.substr);
        }
        break;
    case MSE_MATCH_NAME:
        if (match_data->is_regex) {
            matches = mse_card_name_matches((mse_card_t *) node->payload, data.re);
        } else {
            // this is probably not going to be ussed but eh
            matches = mse_str_match(((mse_card_t *) node->payload)->name_lower, data.substr);
        }
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

static void __mse_match_card_node(avl_tree_node_t *node, mse_card_match_t *match_data, mse_card_match_cmp_data_t cmp_data)
{
    if (node == NULL) {
        return;
    }

    __mse_match_card_do_match(node, match_data, cmp_data);
    __mse_match_card_node(node->l, match_data, cmp_data);
    __mse_match_card_node(node->r, match_data, cmp_data);
}

static void __mse_match_card_worker(void *data, thread_pool_t *pool)
{
    mse_card_match_worker_data_t *match_data = (mse_card_match_worker_data_t *) data;

    regex_t re;
    mse_card_match_cmp_data_t cmp_data;
    if (match_data->match_data->is_regex) {
        if (!mse_compile_regex(match_data->match_data->regex, &re)) {
            match_data->match_data->err = 1;
            lprintf(LOG_ERROR, "Cannot compile regex\n");
            goto cleanup;
        }
        cmp_data.re = &re;
    }

    __mse_match_card_node(match_data->root, match_data->match_data, cmp_data);
    if (match_data->match_data->is_regex) {
        regfree(&re);
    }
cleanup:
    sem_post(&match_data->match_data->sem);
    free(match_data);
}

static int __mse_match_card_worker_enqueue(mse_card_match_t *match_data,
        thread_pool_t *pool,
        avl_tree_node_t *node,
        int h,
        mse_card_match_cmp_data_t data)
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
        __mse_match_card_do_match(node, match_data, data);
    }

    // Recurse
    sum += __mse_match_card_worker_enqueue(match_data, pool, node->l, h - 1, data);
    sum += __mse_match_card_worker_enqueue(match_data, pool, node->r, h - 1, data);
    return sum;
}

static int __mse_match_cards(avl_tree_node_t **ret,
                             avl_tree_node_t *cards_tree,
                             char *str,
                             int is_regex,
                             thread_pool_t *pool,
                             mse_card_match_type_t type)
{
    regex_t re;
    mse_card_match_cmp_data_t cmp_data;
    if (is_regex) {
        ASSERT(mse_compile_regex(str, &re));
        cmp_data.re = &re;
    } else {
        ASSERT(cmp_data.substr = mse_to_lower(str));
    }

    mse_card_match_t data;
    memset(&data, 0, sizeof(data));
    data.type = type;
    data.res = ret;
    data.regex = str;
    data.is_regex = is_regex;

    pthread_mutex_t lock_tmp = PTHREAD_MUTEX_INITIALIZER;
    data.lock = lock_tmp;

    int layers = (int) floor(log2(pool->threads_count));
    sem_init(&data.sem, 0, 0); // Each thread calls up()
    int thread_cnt = __mse_match_card_worker_enqueue(&data, pool, cards_tree, layers, cmp_data);

    // Wait for the threads then cleanup
    for (int i = 0; i < thread_cnt; i++) {
        int waiting = 1;
        while (waiting) {
            pool_try_consume(pool);
            waiting = sem_trywait(&data.sem) != 0;
        }
    }

    if (is_regex) {
       regfree(&re);
    }
    pthread_mutex_destroy(&data.lock);

    ASSERT(data.err == 0);
    return 1;
}

int mse_matching_card_oracle(avl_tree_node_t **ret,
                             avl_tree_node_t *cards_tree,
                             char *str,
                             int is_regex,
                             thread_pool_t *pool)
{
    return __mse_match_cards(ret, cards_tree, str, is_regex, pool, MSE_MATCH_ORACLE);
}

int mse_matching_card_name(avl_tree_node_t **ret,
                           avl_tree_node_t *cards_tree,
                           char *str,
                           int is_regex,
                           thread_pool_t *pool)
{
    return __mse_match_cards(ret, cards_tree, str, is_regex, pool, MSE_MATCH_NAME);
}

char *escape_regex(char *regex)
{
    size_t len = strlen(regex);
    ASSERT(len > 1);

    char *ret = malloc(len + 1);
    ASSERT(ret != NULL);

    size_t offset = 0;
    int escaping = regex[0] == '/' && regex[len - 1] == '/';
    if (escaping) {
        offset++;
    }

    strcpy(ret, &regex[offset]);

    if (escaping) {
        size_t r_end = len - 1 - offset;
        if (ret[r_end] == '/') {
            ret[r_end] = 0;
        }
    }

    return ret;
}
