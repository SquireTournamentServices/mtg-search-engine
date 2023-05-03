/*
 * This is probably aweful practice but this one file will contain the entire query parser!
 * It is for use with BISON, idk what I am doing please Adrian Johnstone send me some help.
 */
%{
#include <stdlib.h>
#include <string.h>
#include "testing_h/testing.h"
#include "src/interpretor.h"

/// Forward declare to suppress errors
void yyerror(const char *s);

static mse_set_generator_operator_t yy_parser_operator;
%}

/// Name part of a set generator in a query i.e: 'colour'
%token OP_NAME
/// The operand i.e: '<=', '<', ...
%token OP_OPERATOR
/// An argument for an op
%token OP_ARGUMENT
/// An bivariable function that operates on two input sets i.e: 'and' (which is set intersection)
%token OPERATOR

// Token match definitions
%%
operand: "<=" { yy_parser_operator = MSE_SET_GENERATOR_OP_LT_INC; return MSE_PARSER_OP_OPERATOR; }
       | "<" { yy_parser_operator = MSE_SET_GENERATOR_OP_LT; return MSE_PARSER_OP_OPERATOR; }
       | ">" { yy_parser_operator = MSE_SET_GENERATOR_OP_GT; return MSE_PARSER_OP_OPERATOR; }
       | ">=" { yy_parser_operator = MSE_SET_GENERATOR_OP_GT_INC; return MSE_PARSER_OP_OPERATOR; }
       | ":" { yy_parser_operator = MSE_SET_GENERATOR_OP_INCLUDES; return MSE_PARSER_OP_OPERATOR; }
       | "=" { yy_parser_operator = MSE_SET_GENERATOR_OP_EQUALS; return MSE_PARSER_OP_OPERATOR; }
       ;

%%

/// Functions
void yyerror(const char *s)
{
    lprintf(LOG_ERROR, "Parse error: %s\n", s);
}
