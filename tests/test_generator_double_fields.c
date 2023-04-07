#include "./test_generators.h"
#include "../testing_h/testing.h"
#include "../src/avl_tree.h"
#include "../src/generators.h"
#include <string.h>
#include <math.h>

#define BAD_ARGUMENT "12341234123412341234sjaskldjaslkdjasld :()"
#define DEFAULT_ARGUMENT "5"
#define DEFAULT_ARGUMENT_DOUBLE atof(DEFAULT_ARGUMENT)

static int found_eq;

#define TEST_FEILD_GENERATORS_FUNC_FOR(fname, type_name_suffix) \
static int __test_generator_##fname(mse_set_generator_operator_t op_type, int (*test_tree)(avl_tree_node_t *node)) \
{ \
    mse_set_generator_type_t gen_type = MSE_SET_GENERATOR_##type_name_suffix; \
    size_t len = strlen(DEFAULT_ARGUMENT); \
 \
    mse_set_generator_t ret; \
    ASSERT(mse_init_set_generator(&ret, gen_type, op_type, DEFAULT_ARGUMENT, len)); \
 \
    avl_tree_node_t *root = NULL; \
    ASSERT(mse_generate_set(&ret, &root, &gen_cards, &gen_thread_pool)); \
    ASSERT(tree_size(root) > 0); \
    ASSERT(test_tree(root)); \
    free_tree(root); \
    mse_free_set_generator(&ret); \
    return 1; \
} \
 \
static int test_generator_##fname##_invalid_arg() \
{ \
    mse_set_generator_operator_t op_type = MSE_SET_GENERATOR_OP_LT; \
    mse_set_generator_type_t gen_type = MSE_SET_GENERATOR_##type_name_suffix; \
    size_t len = strlen(BAD_ARGUMENT); \
 \
    mse_set_generator_t ret; \
    ASSERT(mse_init_set_generator(&ret, gen_type, op_type, BAD_ARGUMENT, len)); \
 \
    avl_tree_node_t *root = NULL; \
    ASSERT(mse_generate_set(&ret, &root, &gen_cards, &gen_thread_pool) == 0); \
    mse_free_set_generator(&ret); \
    return 1; \
} \
 \
static int test_tree_##fname##_eq(avl_tree_node_t *node) \
{ \
    if (node == NULL) { \
        return 1; \
    } \
 \
    double fname = ((mtg_card_t *) node->payload)->fname; \
    ASSERT((int) DEFAULT_ARGUMENT_DOUBLE == (int) fname); \
    ASSERT(test_tree_##fname##_eq(node->l)); \
    ASSERT(test_tree_##fname##_eq(node->r)); \
    return 1; \
} \
 \
static int test_generator_##fname##_eq() \
{ \
    ASSERT(__test_generator_##fname(MSE_SET_GENERATOR_OP_EQUALS, test_tree_##fname##_eq)); \
    return 1; \
} \
 \
static int test_generator_##fname##_inc() \
{ \
    ASSERT(__test_generator_##fname(MSE_SET_GENERATOR_OP_INCLUDES, test_tree_##fname##_eq)); \
    return 1; \
} \
 \
static int test_tree_##fname##_lt(avl_tree_node_t *node) \
{ \
    if (node == NULL) { \
        return 1; \
    } \
 \
    double fname = ((mtg_card_t *) node->payload)->fname; \
    ASSERT(fname < DEFAULT_ARGUMENT_DOUBLE); \
    ASSERT(fabs(DEFAULT_ARGUMENT_DOUBLE - fname) > 0.01); \
    ASSERT(test_tree_##fname##_lt(node->l)); \
    ASSERT(test_tree_##fname##_lt(node->r)); \
    return 1; \
} \
 \
static int test_generator_##fname##_lt() \
{ \
    ASSERT(__test_generator_##fname(MSE_SET_GENERATOR_OP_LT, test_tree_##fname##_lt)); \
    return 1; \
} \
 \
static int test_tree_##fname##_lt_inc(avl_tree_node_t *node) \
{ \
    if (node == NULL) { \
        return 1; \
    } \
 \
    double fname = ((mtg_card_t *) node->payload)->fname; \
    ASSERT(fname <= DEFAULT_ARGUMENT_DOUBLE ); \
    found_eq |= (int) DEFAULT_ARGUMENT_DOUBLE == (int) fname; \
    ASSERT(test_tree_##fname##_lt_inc(node->l)); \
    ASSERT(test_tree_##fname##_lt_inc(node->r)); \
    return 1; \
} \
 \
static int test_generator_##fname##_lt_inc() \
{ \
    found_eq = 0; \
    ASSERT(__test_generator_##fname(MSE_SET_GENERATOR_OP_LT_INC, &test_tree_##fname##_lt_inc)); \
    ASSERT(found_eq); \
    return 1; \
} \
 \
static int test_tree_##fname##_gt(avl_tree_node_t *node) \
{ \
    if (node == NULL) { \
        return 1; \
    } \
 \
    double fname = ((mtg_card_t *) node->payload)->fname; \
    ASSERT(fname > DEFAULT_ARGUMENT_DOUBLE); \
    ASSERT(fabs(DEFAULT_ARGUMENT_DOUBLE - fname) > 0.01); \
    ASSERT(test_tree_##fname##_gt(node->l)); \
    ASSERT(test_tree_##fname##_gt(node->r)); \
    return 1; \
} \
 \
static int test_generator_##fname##_gt() \
{ \
    ASSERT(__test_generator_##fname(MSE_SET_GENERATOR_OP_GT, &test_tree_##fname##_gt)); \
    return 1; \
} \
 \
static int test_tree_##fname##_gt_inc(avl_tree_node_t *node) \
{ \
    if (node == NULL) { \
        return 1; \
    } \
 \
    double fname = ((mtg_card_t *) node->payload)->fname; \
    ASSERT(fname >= DEFAULT_ARGUMENT_DOUBLE); \
    found_eq |= (int) DEFAULT_ARGUMENT_DOUBLE == (int) fname; \
    ASSERT(test_tree_##fname##_gt_inc(node->l)); \
    ASSERT(test_tree_##fname##_gt_inc(node->r)); \
    return 1; \
} \
 \
static int test_generator_##fname##_gt_inc() \
{ \
    found_eq = 0; \
    ASSERT(__test_generator_##fname(MSE_SET_GENERATOR_OP_GT_INC, &test_tree_##fname##_gt_inc)); \
    ASSERT(found_eq); \
    return 1; \
}

#define TEST_FEILD_GENERATORS_FOR(fname) \
{&test_generator_##fname##_invalid_arg, "Test generator " #fname " invalid arg"}, \
{&test_generator_##fname##_eq, "Test generator " #fname " ="}, \
{&test_generator_##fname##_inc, "Test generator " #fname " :"}, \
{&test_generator_##fname##_lt, "Test genreator " #fname " <"}, \
{&test_generator_##fname##_lt_inc, "Test genreator " #fname " <="}, \
{&test_generator_##fname##_gt, "Test genreator " #fname " >"}, \
{&test_generator_##fname##_gt_inc, "Test genreator " #fname " >="}

TEST_FEILD_GENERATORS_FUNC_FOR(power, POWER)
TEST_FEILD_GENERATORS_FUNC_FOR(toughness, TOUGHNESS)
TEST_FEILD_GENERATORS_FUNC_FOR(cmc, CMC)

SUB_TEST(test_set_generators_double_fields, TEST_FEILD_GENERATORS_FOR(power),
         TEST_FEILD_GENERATORS_FOR(toughness),
         TEST_FEILD_GENERATORS_FOR(cmc))