#include "./test_avl_tree.h"
#include "../testing_h/testing.h"
#include "../src/avl_tree.h"
#include <string.h>
#include <time.h>

static int __test_heights(tree_node *root, size_t h)
{
    if (root == NULL) {
        return 1;
    }

    ASSERT(root->height < h);
    ASSERT(__test_heights(root->l, h - 1));
    ASSERT(__test_heights(root->r, h - 1));
    return 1;
}

static int test_heights(tree_node *root)
{
    return __test_heights(root, root->height + 1);
}


static int test_tree_props(tree_node *node)
{
    if (node == NULL) {
        return 1;
    }

    if (node->l != NULL) {
        ASSERT(node->cmp_payload(node->payload, node->l->payload) <= 0);
        ASSERT(test_tree_props(node->l));
    }

    if (node->r != NULL) {
        ASSERT(node->cmp_payload(node->payload, node->r->payload) > 0);
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
    tree_node *node = init_tree_node(NULL, &cmp_size_t, payload);
    ASSERT(node != NULL);
    ASSERT(node->height == 1);
    ASSERT(node->payload == payload);
    ASSERT(node->free_payload == NULL);
    ASSERT(node->cmp_payload == &cmp_size_t);
    ASSERT(find_payload(node, node->payload));
    free_tree(node);

    return 1;
}

// There will be about 30,000 MTG cards at some point, it should be perform somewhat well with them
#define MAX_NODES 30000
#define MAX_TIME 3

static int test_tree_insert()
{
    time_t t1 = time(NULL);
    void *payload = (void *) 1L;
    tree_node *tree = init_tree_node(NULL, &cmp_size_t, payload);
    ASSERT(tree != NULL);
    ASSERT(tree->cmp_payload == &cmp_size_t);
    ASSERT(tree->free_payload == NULL);
    ASSERT(tree->payload == payload);
    ASSERT(find_payload(tree, payload));

    // Add to tree
    for (size_t i = 0; i < MAX_NODES; i++) {
        void *ptr = (void *) random();
        tree_node *node = init_tree_node(NULL, &cmp_size_t, ptr);
        ASSERT(node != NULL);

        ASSERT(ptr == node->payload);
        ASSERT(node->cmp_payload(node->payload, ptr) == 0);

        ASSERT(find_payload(tree, (void *) node->payload) == 0);
        ASSERT(insert_node(tree, node));
        ASSERT(find_payload(tree, (void *) node->payload));
    }

    time_t t2 = time(NULL);
    ASSERT(t2 - t1 <= MAX_TIME);

    lprintf(LOG_INFO, "Tree height %lu for %lu nodes\n", tree->height, MAX_NODES);
    ASSERT(test_heights(tree));
    ASSERT(test_tree_props(tree));

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

    tree_node *tree = init_tree_node(&free, &cmp_int_pointer, ptr);
    ASSERT(tree != NULL);

    // Add to tree
    for (size_t i = 0; i < MAX_NODES; i++) {
        int *ptr = malloc(sizeof(*ptr));
        ASSERT(ptr != NULL);
        *ptr = random();

        tree_node *node = init_tree_node(&free, &cmp_int_pointer, ptr);
        ASSERT(node != NULL);

        ASSERT(insert_node(tree, node));
    }

    time_t t2 = time(NULL);
    ASSERT(t2 - t1 <= MAX_TIME);

    lprintf(LOG_INFO, "Tree height %lu for %lu node\n", tree->height, MAX_NODES);
    ASSERT(test_heights(tree));
    ASSERT(test_tree_props(tree));

    free_tree(tree);
    return 1;
}

static int test_tree_insert_3()
{
    time_t t1 = time(NULL);
    tree_node *tree = init_tree_node(NULL, &cmp_size_t, (void *) 1L);
    ASSERT(tree != NULL);

    // Add to tree
    for (size_t i = 0; i < 16; i++) {
        tree_node *node = init_tree_node(NULL, &cmp_size_t, (void *) random());
        ASSERT(node != NULL);

        ASSERT(insert_node(tree, node));
    }

    time_t t2 = time(NULL);
    ASSERT(t2 - t1 <= MAX_TIME);

    print_tree(tree);
    lprintf(LOG_INFO, "Tree height %lu for %lu nodes\n", tree->height, MAX_NODES);
    ASSERT(test_heights(tree));
    ASSERT(test_tree_props(tree));

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
