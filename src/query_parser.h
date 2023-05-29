#pragma once
#include "./interpretor.h"

/// Internal struct for the status of the parser
typedef struct mse_parser_status_t {
    mse_set_generator_operator_t parser_op_operator;
    mse_set_operator_type_t parser_operator;
    char *tmp_buffer;
    char *op_name_buffer;
    char *argument_buffer;
    mse_interp_node_t *node, *root, *tree_write;
} mse_parser_status_t;

int parse_input_string(const char* input_string, mse_interp_node_t *root);
