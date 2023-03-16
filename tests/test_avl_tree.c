#include "./test_avl_tree.h"
#include "../testing_h/testing.h"
#include "../src/avl_tree.h"
#include <string.h>
#include <time.h>

static int __test_heights(avl_tree_node *root, size_t h)
{
    if (root == NULL) {
        return 1;
    }

    ASSERT(root->height < h);
    ASSERT(__test_heights(root->l, h - 1));
    ASSERT(__test_heights(root->r, h - 1));
    return 1;
}

static int test_heights(avl_tree_node *root)
{
    return __test_heights(root, root->height + 1);
}


static int test_tree_props(avl_tree_node *node)
{
    if (node == NULL) {
        return 1;
    }

    if (node->l != NULL) {
        ASSERT(node->cmp_payload(node->l->payload, node->payload) <= 0);
        ASSERT(test_tree_props(node->l));
    }

    if (node->r != NULL) {
        ASSERT(node->cmp_payload(node->r->payload, node->payload) > 0);
        ASSERT(test_tree_props(node->r));
    }

    return 1;
}

static int cmp_size_t(void *_a, void *_b)
{
    size_t a = (size_t) _a;
    size_t b = (size_t) _b;

    if (a == b) {
        return 0;
    } else if (a < b) {
        return -1;
    }
    return 1;
}

static int test_tree_init_free()
{
    void *payload = (void *) 1L;
    avl_tree_node *node = init_avl_tree_node(NULL, &cmp_size_t, payload);
    ASSERT(node != NULL);
    ASSERT(node->height == 1);
    ASSERT(node->payload == payload);
    ASSERT(node->free_payload == NULL);
    ASSERT(node->cmp_payload == &cmp_size_t);
    ASSERT(find_payload(node, node->payload));
    ASSERT(find_payload(node, payload));
    print_tree(node);
    free_tree(node);

    return 1;
}

#define MAX_NODES 100000
#define MAX_TIME 3

static int test_tree_insert()
{
    time_t t1 = time(NULL);
    void *payload = (void *) 1L;
    avl_tree_node *tree = init_avl_tree_node(NULL, &cmp_size_t, payload);
    ASSERT(tree != NULL);
    ASSERT(tree->cmp_payload == &cmp_size_t);
    ASSERT(tree->free_payload == NULL);
    ASSERT(tree->payload == payload);
    ASSERT(find_payload(tree, payload));

    // Add to tree
    size_t cnt = 0;
    for (size_t i = 0; i < MAX_NODES; i++) {
        void *ptr = (void *) random();
        avl_tree_node *node = init_avl_tree_node(NULL, &cmp_size_t, ptr);
        ASSERT(node != NULL);

        ASSERT(ptr == node->payload);
        ASSERT(node->cmp_payload(node->payload, ptr) == 0);

        if (find_payload(tree, (void *) node->payload) != 0) {
            free_tree(node);
            cnt++;
            continue;
        }
        ASSERT(insert_node(&tree, node));
        ASSERT(tree != NULL);
        ASSERT(find_payload(tree, ptr));
        ASSERT(find_payload(tree, node->payload));
    }

    time_t t2 = time(NULL);
    ASSERT(t2 - t1 <= MAX_TIME);

    lprintf(LOG_INFO, "Tree height %lu for %lu nodes\n", tree->height, MAX_NODES);
    ASSERT(test_heights(tree));
    ASSERT(test_tree_props(tree));
    ASSERT(cnt < MAX_NODES / 10);

    free_tree(tree);
    return 1;
}

static int cmp_int_pointer(void *_a, void *_b)
{
    int a = *(int *) _a;
    int b = *(int *) _b;

    if (a == b) {
        return 0;
    } else if (a < b) {
        return -1;
    }
    return 1;
}

static int test_tree_insert_2()
{
    time_t t1 = time(NULL);
    int *ptr = malloc(sizeof(*ptr));
    ASSERT(ptr != NULL);
    *ptr = random();

    avl_tree_node *tree = init_avl_tree_node(&free, &cmp_int_pointer, ptr);
    ASSERT(tree != NULL);

    // Add to tree
    size_t cnt = 0;
    for (size_t i = 0; i < MAX_NODES; i++) {
        int *ptr = malloc(sizeof(*ptr));
        ASSERT(ptr != NULL);
        *ptr = random();

        avl_tree_node *node = init_avl_tree_node(&free, &cmp_int_pointer, ptr);
        ASSERT(node != NULL);

        if (find_payload(tree, (void *) node->payload) != 0) {
            free_tree(node);
            cnt++;
            continue;
        }
        ASSERT(insert_node(&tree, node));
    }

    time_t t2 = time(NULL);
    ASSERT(t2 - t1 <= MAX_TIME);

    lprintf(LOG_INFO, "Tree height %lu for %lu node\n", tree->height, MAX_NODES);
    ASSERT(test_heights(tree));
    ASSERT(test_tree_props(tree));
    ASSERT(cnt < MAX_NODES / 10);

    free_tree(tree);
    return 1;
}

static int test_tree_insert_3()
{
    time_t t1 = time(NULL);
    avl_tree_node *tree = init_avl_tree_node(NULL, &cmp_size_t, (void *) 1L);
    ASSERT(tree != NULL);

    // Add to tree
    size_t cnt = 0;
    for (size_t i = 0; i < 16; i++) {
        avl_tree_node *node = init_avl_tree_node(NULL, &cmp_size_t, (void *) random());
        ASSERT(node != NULL);

        if (find_payload(tree, (void *) node->payload) != 0) {
            free_tree(node);
            cnt++;
            continue;
        }
        ASSERT(insert_node(&tree, node));
    }

    time_t t2 = time(NULL);
    ASSERT(t2 - t1 <= MAX_TIME);

    ASSERT(cnt < MAX_NODES / 10);
    lprintf(LOG_INFO, "Tree height %lu for %lu nodes\n", tree->height, MAX_NODES);
    ASSERT(test_heights(tree));
    ASSERT(test_tree_props(tree));
    print_tree(tree);

    free_tree(tree);
    return 1;
}

static int test_find_payload_null()
{
    ASSERT(find_payload(NULL, (void *) 1L) == 0);
    return 1;
}

SUB_TEST(test_avl_tree, {&test_tree_init_free, "Test AVL tree init free"},
{&test_tree_insert, "Test tree insert"},
{&test_tree_insert_2, "Test tree insert 2"},
{&test_tree_insert_3, "Test tree insert 3"},
{&test_find_payload_null, "Test find payload for NULL case"})
