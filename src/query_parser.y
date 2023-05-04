/*
 * This is probably aweful practice but this one file will contain the entire query parser!
 * It is for use with BISON, idk what I am doing please Adrian Johnstone send me some help.
 */
%{
#include <stdlib.h>
#include <string.h>
#include "testing_h/testing.h"
#undef lprintf
#define lprintf fprintf(LOG_STREAM, "(" ANSI_YELLOW "%s" ANSI_RESET \
                        ":" ANSI_YELLOW "%d" ANSI_RESET ") \t", __FILE__, __LINE__ ),\
                        __lprintf
#include "src/interpretor.h"

static mse_set_generator_operator_t yy_parser_op_operator = -1;
static char *tmp_buffer = NULL;
static char *op_name_buffer = NULL;

/// Forward declare to suppress errors
void yyerror(const char *s);
%}

/// Name part of a set generator in a query i.e: 'colour'
%token OP_NAME
/// An argument for an op
%token OP_ARGUMENT
/// An bivariable function that operates on two input sets i.e: 'and' (which is set intersection)
%token OPERATOR

%token WORD

%{
#define WORD_REGEX "[a-zA-Z0-9]+"
%}

// Token match definitions
%%
op_operator : "<=" { yy_parser_op_operator = MSE_SET_GENERATOR_OP_LT_INC; }
                  | "<" { yy_parser_op_operator = MSE_SET_GENERATOR_OP_LT; }
                  | ">" { yy_parser_op_operator = MSE_SET_GENERATOR_OP_GT; }
                  | ">=" { yy_parser_op_operator = MSE_SET_GENERATOR_OP_GT_INC; }
                  | ":" { yy_parser_op_operator = MSE_SET_GENERATOR_OP_INCLUDES; }
                  | "=" { yy_parser_op_operator = MSE_SET_GENERATOR_OP_EQUALS; }
                  ;

word: WORD {
    /* Allocate a new buffer for the word */
    tmp_buffer = (char*) malloc(sizeof(char) * (yyleng + 1));
    ASSERT(tmp_buffer != NULL);
    /* Copy the word to the temporary buffer */
    strncpy(tmp_buffer, yytext, yyleng);
    tmp_buffer[yyleng] = '\0';
    }

op_name: word {
       /* Allocate a new buffer for the op_name */
       op_name_buffer = (char*) malloc(sizeof(char) * (strlen(tmp_buffer) + 1));
       ASSERT(op_name_buffer != NULL);
       /* Copy the word from the temporary buffer to the op_name buffer */
       strcpy(op_name_buffer, tmp_buffer);
       free(tmp_buffer);
       tmp_buffer = NULL;
       }

%%

void yyerror(const char *s)
{
    lprintf(LOG_ERROR, "Parse error: %s\n", s);
}
