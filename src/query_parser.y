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

static mse_set_generator_operator_t parser_op_operator = -1;
static mse_set_operator_t parser_operator = -1;
static char *tmp_buffer = NULL;
static char *op_name_buffer = NULL;
static char *argument_buffer = NULL;

/// Forward declare to suppress errors
void yyerror(const char *s);
%}

/// Name part of a set generator in a query i.e: 'colour'
%token OP_NAME
/// An bivariable function that operates on two input sets i.e: 'and' (which is set intersection)
%token OPERATOR

%token WORD
%token STRING
%token REGEX_STRING

%token AND
%token OR

%{
#define WORD_REGEX "[a-zA-Z0-9]+"
#define STRING_REGEX "\"([^\"]|(\\\\\"))+\""
#define REGEX_STRING_REGEX "\/([^\/]|(\\\\\/))+\/"

#define AND_REGEX "[aA][nN][dD]"
#define OR_REGEX "[oO][rR]"

#define COPY_TO_TMP_BUFFER \
    tmp_buffer = (char*) malloc(sizeof(char) * (yyleng + 1)); \
    ASSERT(tmp_buffer != NULL); \
    strncpy(tmp_buffer, yytext, yyleng); \
    tmp_buffer[yyleng] = '\0'; \
    
#define COPY_TO_ARGUMENT_BUFFER \
    ASSERT(argument_buffer = strdup(tmp_buffer)); \
    free(tmp_buffer); \
    tmp_buffer = NULL;

static int __mse_handle_set_generator()
{
    // TODO: Create a set_generator object and put it in the tree
    return 1;
}

/// Calls the handler for a set generator then cleans the internal state
static int mse_handle_set_generator()
{
    int r = __mse_handle_set_generator();
    free(tmp_buffer);
    tmp_buffer = NULL;

    free(op_name_buffer);
    op_name_buffer = NULL;

    free(argumen_buffer);
    argument_buffer = NULL;

    parser_op_operator = -1;  
    return r;
}

%}

// Token match definitions
%%
op_operator : "<=" { parser_op_operator = MSE_SET_GENERATOR_OP_LT_INC; }
            | "<" { parser_op_operator = MSE_SET_GENERATOR_OP_LT; }
            | ">" { parser_op_operator = MSE_SET_GENERATOR_OP_GT; }
            | ">=" { parser_op_operator = MSE_SET_GENERATOR_OP_GT_INC; }
            | ":" { parser_op_operator = MSE_SET_GENERATOR_OP_INCLUDES; }
            | "=" { parser_op_operator = MSE_SET_GENERATOR_OP_EQUALS; }
            ;

word: WORD {
    COPY_TO_TMP_BUFFER
    }

op_name: word {
       ASSERT(op_name_buffer = strdup(tmp_buffer));
       free(tmp_buffer);
       tmp_buffer = NULL;
       }

string: STRING {
      COPY_TO_TMP_BUFFER
      }

regex_string: REGEX_STRING {
            COPY_TO_TMP_BUFFER
            }

op_argument: string { ; }
           | regex_string { ; }
           | word { ; }
           ;

set_generator: op_name op_operator op_operator { mse_handle_set_generator(); }

operator : AND { parser_operator = MSE_SET_INTERSECTION; }
         | OR { parser_operator = MSE_SET_UNION; }
         ;

%%

void yyerror(const char *s)
{
    lprintf(LOG_ERROR, "Parse error: %s\n", s);
}
