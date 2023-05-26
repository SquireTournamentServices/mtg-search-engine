#include "./query_builder.h"
#include "../testing_h/testing.h"
#include <string.h>
#include <stdlib.h>

#define MSE_STACK_INC_SIZE 100

static int __mse_expand_stack(mse_query_builder_t *builder)
{
    if (builder->stack == NULL) {
        ASSERT(builder->stack =
                   malloc(sizeof(*builder->stack) * MSE_STACK_INC_SIZE));
    } else {
        ASSERT(builder->stack = realloc(builder->stack,
                                        sizeof(*builder->stack) * (MSE_STACK_INC_SIZE + builder->stack_len)));
    }
    builder->stack_len += MSE_STACK_INC_SIZE;
    return 1;
}

static int __mse_push_stack(mse_query_builder_t *builder,
                            mse_query_builder_stack_entry_t entry)
{
    if (builder->stack_ptr + 1 >= builder->stack_len) {
        ASSERT(__mse_expand_stack(builder));
    }

    builder->stack[builder->stack_ptr++] = entry;
    return 1;
}

static int __mse_pop_stack(mse_query_builder_t *builder,
                           mse_query_builder_stack_entry_t *ret)
{
    ASSERT(builder->stack_ptr > 0);
    *ret = builder->stack[--builder->stack_ptr];
    return 1;
}

static int __mse_peek_stack(mse_query_builder_t *builder,
                            mse_query_builder_stack_entry_t *ret)
{
    ASSERT(builder->stack_ptr > 0);
    *ret = builder->stack[builder->stack_ptr - 1];
    return 1;
}

static inline size_t __mse_size_stack(mse_query_builder_t *builder)
{
    return builder->stack_ptr;
}

int mse_init_query_builder(mse_query_builder_t *builder)
{
    memset(builder, 0, sizeof(*builder));
    ASSERT(__mse_expand_stack(builder));
    return 1;
}

int mse_query_builder_add_node(mse_query_builder_t *builder,
                               mse_interp_node_t *node)
{
    mse_query_builder_stack_entry_t entry;
    memset(&entry, 0, sizeof(entry));
    entry.node = node;

    ASSERT(__mse_push_stack(builder, entry));
    return 1;
}

int mse_query_builder_enter_statement(mse_query_builder_t *builder)
{
    mse_query_builder_stack_entry_t entry;
    memset(&entry, 0, sizeof(entry));
    entry.is_open_bracket = 1;

    ASSERT(__mse_push_stack(builder, entry));
    return 1;
}

static int __mse_stmt_finished(mse_query_builder_t *builder,
                               int *ret)
{
    if (builder->stack_ptr == 0) {
        return 1;
    }

    mse_query_builder_stack_entry_t entry;
    ASSERT(__mse_peek_stack(builder, &entry));
    if (entry.is_open_bracket) {
        ASSERT(__mse_pop_stack(builder, &entry));
        return 1;
    }

    return 1;
}

static int ptr_cmp(void *a, void *b)
{
    if (a < b) {
        return -1;
    } else if (a > b) {
        return 1;
    }
    return 0;
}

static int __mse_append_node_r(mse_query_builder_t *builder,
                               avl_tree_node_t **root,
                               mse_interp_node_t *node)
{
    // Guard against empty stack
    if (__mse_size_stack(builder) == 0) {
        return 1;
    }

    // Guard against open bracket
    mse_query_builder_stack_entry_t entry;
    ASSERT(__mse_peek_stack(builder, &entry));
    if (entry.is_open_bracket) {
        return 1;
    }

    // If the next node is a set generator then pop it, recursively handle this, then readd it
    avl_tree_node_t *avl_node = NULL;
    ASSERT(entry.node != NULL);
    switch (entry.node->type) {
    case MSE_INTERP_NODE_SET_GENERATOR:
        ASSERT(__mse_pop_stack(builder, &entry));
        ASSERT(__mse_append_node_r(builder, root, node));
        ASSERT(__mse_push_stack(builder, entry));
        break;
    case MSE_INTERP_NODE_SET_OPERATOR:
        // If it is not the same operator as the last one then return as the
        if (entry.node->op_type != node->op_type) {
            return 1;
        }

        ASSERT(avl_node = init_avl_tree_node(NULL, &ptr_cmp, entry.node));
        ASSERT(insert_node(root, avl_node));
        ASSERT(__mse_pop_stack(builder, &entry));
        ASSERT(__mse_append_node_r(builder, root, entry.node));
        break;
    case MSE_INTERP_NODE_SET_CONSUMER:
        lprintf(LOG_ERROR, "Fix me.\n");
        return 0;
        break;
    }
    return 1;
}

static int __mse_add_avl_node(mse_query_builder_t *builder,
                              avl_tree_node_t *root,
                              int *cnt)
{
    if (root == NULL) {
        return 1;
    }

    *cnt++;
    ASSERT(__mse_add_avl_node(builder, root->l, cnt));
    ASSERT(__mse_add_avl_node(builder, root->r, cnt));
    return 1;
}

static int __mse_append_node(mse_query_builder_t *builder)
{
    avl_tree_node_t *root = NULL;
    int rc = 0;
    if (!__mse_append_node_r(builder, &root, NULL)) {
        goto cleanup;
    }

    // Append the balanced tree to the parse tree
    int cnt = 0;
    ASSERT(__mse_add_avl_node(builder, root, &cnt));

    // Set the return code to success and cleanup
    ASSERT(root != NULL);
    rc = 1;
cleanup:
    if (!rc) {
        lprintf(LOG_ERROR, "Cannot append node.\n");
    }
    free_tree(root);
    return rc;
}

int mse_query_builder_exit_statement(mse_query_builder_t *builder)
{
    int r = 1;
    while (r) {
        ASSERT(__mse_stmt_finished(builder, &r));
        ASSERT(__mse_append_node(builder));
    }
    return 1;
}

static void __mse_free_query_builder(mse_query_builder_t *builder)
{
    if (builder->stack != NULL) {
        free(builder->stack);
    }
    memset(builder, 0, sizeof(*builder));
}

int mse_finalise_query_builder(mse_query_builder_t *builder,
                               mse_interp_node_t **ret)
{
    __mse_free_query_builder(builder);
    return 1;
}
