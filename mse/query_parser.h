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
} mse_parser_status_t;

typedef struct YYLTYPE {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
} YYLTYPE;

int mse_parse_input_string(const char* input_string, mse_interp_node_t **root);
