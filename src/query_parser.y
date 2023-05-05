%{
#include <stdlib.h>
#include <string.h>
#include "testing_h/testing.h"
#undef lprintf
#define lprintf fprintf(LOG_STREAM, "(" ANSI_YELLOW "%s" ANSI_RESET \
                        ":" ANSI_YELLOW "%d" ANSI_RESET ") \t", __FILE__, __LINE__ ),\
                        __lprintf
#include "src/interpretor.h"
#include "src/query_parser.h"
#include "mse_query_lexer.h"
#include "mse_query_parser.h"

static mse_set_generator_operator_t parser_op_operator;
static mse_set_operator_type_t parser_operator;
static char *tmp_buffer = NULL;
static char *op_name_buffer = NULL;
static char *argument_buffer = NULL;

static void yyerror(const char *s)
{
    lprintf(LOG_ERROR, "Parse error: %s\n", s);
}
%}

%token LT
%token LT_INC
%token GT
%token GT_INC
%token INCLUDES
%token EQUALS

%token WORD
%token STRING

%token REGEX_STRING

%token AND
%token OR

%token WHITESPACE

%token OPEN_BRACKET
%token CLOSE_BRACKET

%token STMT_NEGATE
%{
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
static int mse_handle_set_generator(int negate)
{
    int r = __mse_handle_set_generator();
    free(tmp_buffer);
    tmp_buffer = NULL;

    free(op_name_buffer);
    op_name_buffer = NULL;

    free(argument_buffer);
    argument_buffer = NULL;
    return r;
}

%}

// Token match definitions
%%
input: query
     ;

op_operator : LT_INC { parser_op_operator = MSE_SET_GENERATOR_OP_LT_INC; }
            | LT { parser_op_operator = MSE_SET_GENERATOR_OP_LT; }
            | GT { parser_op_operator = MSE_SET_GENERATOR_OP_GT; }
            | GT_INC { parser_op_operator = MSE_SET_GENERATOR_OP_GT_INC; }
            | INCLUDES { parser_op_operator = MSE_SET_GENERATOR_OP_INCLUDES; }
            | EQUALS { parser_op_operator = MSE_SET_GENERATOR_OP_EQUALS; }
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

op_argument: string { COPY_TO_ARGUMENT_BUFFER }
           | regex_string { COPY_TO_ARGUMENT_BUFFER }
           | word { COPY_TO_ARGUMENT_BUFFER }
           ;

set_generator: op_name op_operator op_argument { mse_handle_set_generator(0); }
             | STMT_NEGATE op_name op_operator op_argument { mse_handle_set_generator(1); }
             | word { mse_handle_set_generator(0); }
             | string { mse_handle_set_generator(0); }
             ;

operator : AND { parser_operator = MSE_SET_INTERSECTION; }
         | OR { parser_operator = MSE_SET_UNION; }
         ;

query: %empty
     | set_generator WHITESPACE operator WHITESPACE query
     | set_generator WHITESPACE query
     | set_generator
     | OPEN_BRACKET query CLOSE_BRACKET WHITESPACE query
     | OPEN_BRACKET query CLOSE_BRACKET
     ;
%%

int parse_input_string(const char* input_string) {
    // create a new input buffer and switch to it
    YY_BUFFER_STATE input_buffer = yy_scan_string(input_string);

    // call the parser
    int result = yyparse();

    // free the input buffer
    yy_delete_buffer(input_buffer);

    return result == 0;
}
