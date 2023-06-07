#pragma once
#include "./interpretor.h"

/// Internal struct for the status of the parser
typedef struct mse_parser_status_t {
    mse_set_generator_type_t parser_gen_type;
    mse_set_generator_operator_t parser_op_type;
    char *tmp_buffer;
    char *op_name_buffer;
    char *argument_buffer;
    mse_interp_node_t *node, *root;
    /// Nodes that need to put into the tree
    mse_interp_node_t *set_generator_node, *op_node;
} mse_parser_status_t;

int parse_input_string(const char* input_string, mse_interp_node_t **root);