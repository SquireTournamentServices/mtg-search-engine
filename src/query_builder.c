#include "./query_builder.h"
#include "../testing_h/testing.h"
#include <string.h>
#include <stdlib.h>

#define MSE_STACK_INC_SIZE 100

static int __mse_expand_stack(mse_query_builder_t *builder)
{
    if (builder->stack == NULL) {
        ASSERT(builder->stack = malloc(sizeof(*builder->stack) * MSE_STACK_INC_SIZE));
    } else {
        ASSERT(builder->stack = realloc(builder->stack, sizeof(*builder->stack) * (MSE_STACK_INC_SIZE + builder->stack_len)));
    }
    builder->stack_len += MSE_STACK_INC_SIZE;
    return 1;
}

static int __mse_add_to_stack(mse_query_builder_t *builder, mse_query_builder_stack_entry_t entry)
{
    if (builder->stack_ptr + 1 >= builder->stack_len) {
        ASSERT(__mse_expand_stack(builder));
    }

    builder->stack[builder->stack_ptr++] = entry;
    return 1;
}

int mse_init_query_builder(mse_query_builder_t *builder)
{
    memset(builder, 0, sizeof(*builder));
    ASSERT(__mse_expand_stack(builder));
    return 1;
}

int mse_query_builder_add_node(mse_query_builder_t *builder, mse_interp_node_t *node)
{
    mse_query_builder_stack_entry_t entry;
    memset(&entry, 0, sizeof(entry));
    entry.node = node;

    ASSERT(__mse_add_to_stack(builder, entry));
    return 1;
}

int mse_query_builder_enter_statement(mse_query_builder_t *builder)
{
    mse_query_builder_stack_entry_t entry;
    memset(&entry, 0, sizeof(entry));
    entry.is_open_bracket = 1;

    ASSERT(__mse_add_to_stack(builder, entry));
    return 1;
}

int mse_query_builder_exit_statement(mse_query_builder_t *builder)
{
    // TODO
    return 1;
}

static void __mse_free_query_builder(mse_query_builder_t *builder)
{
    if (builder->stack != NULL) {
        free(builder->stack);
    }
    memset(builder, 0, sizeof(*builder));
}

int mse_finalise_query_builder(mse_query_builder_t *builder, mse_interp_node_t **ret)
{
    __mse_free_query_builder(builder);
    return 1;
}
