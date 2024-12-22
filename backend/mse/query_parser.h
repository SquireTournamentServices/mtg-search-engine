#pragma once
#include "./interpretor.h"

/// Internal struct for the status of the parser
typedef struct mse_parser_status_t {
    mse_set_generator_type_t parser_gen_type;
    mse_set_generator_operator_t parser_op_type;
    char *tmp_buffer;
    char *argument_buffer;
    mse_interp_node_t *node, *root;
    /// Nodes that need to put into the tree
    mse_interp_node_t *set_generator_node, *op_node;
    /// A stack to store the roots of the nested statements
    mse_interp_node_t **stack_roots;
    /// Length of the stack for the nested statements
    size_t stack_roots_len;
    int error;
    // Input that is read from here
    size_t input_buffer_ptr;
    size_t input_buffer_len;
    const char *input_buffer;
} mse_parser_status_t;

int mse_parse_input_string(const char* input_string, mse_interp_node_t **root);

int __mse_handle_set_generator(mse_parser_status_t *ret, int negate);
int mse_handle_set_generator(int negate, mse_parser_status_t *ret);
int __mse_insert_swap_parent(mse_parser_status_t *state,  mse_interp_node_t *node);
int __mse_insert_node_special(mse_parser_status_t *state, mse_interp_node_t *node);
int __mse_insert_node(mse_parser_status_t *state, mse_interp_node_t *node);
int __mse_negate(mse_parser_status_t *state);
int __mse_parser_status_push(mse_parser_status_t *state);
int __mse_parser_status_pop(mse_parser_status_t *state);
void __mse_free_parser_status(mse_parser_status_t *status);
