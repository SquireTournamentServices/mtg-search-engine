%{
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "testing_h/testing.h"
#include "mse/interpretor.h"
#include "mse/query_parser.h"
#include "mse_query_lexer.h"
#include "mse_query_parser.h"
#undef lprintf
#define lprintf fprintf(LOG_STREAM, "(" ANSI_YELLOW "%s" ANSI_RESET \
                        ":" ANSI_YELLOW "%d" ANSI_RESET ") \t", __FILE__, __LINE__ ),\
                        __lprintf
#define PARSE_ASSERT(x) if (!(x)) \
{ \
  lprintf(LOG_ERROR, "Parse Error: Assertion failure, line " \
      ANSI_RED "%d" ANSI_RESET " in " ANSI_RED "%s" ANSI_RESET "\n", \
      __LINE__, ___FNANE); \
  YYABORT; \
}

static pthread_mutex_t parser_lock = PTHREAD_MUTEX_INITIALIZER;

static void yyerror(mse_parser_status_t *__ret, const char *s)
{
    lprintf(LOG_ERROR, "Parse error: %s\n", s);
}
%}

%code requires {
    #include "mse/query_parser.h"
}
%parse-param {mse_parser_status_t *ret}
%glr-parser
%define parse.error verbose

%right LT LT_INC GT GT_INC INCLUDES EQUALS
%right AND OR
%right WORD STRING REGEX_STRING
%right WHITESPACE OPEN_BRACKET CLOSE_BRACKET STMT_NEGATE

%{
#define COPY_TO_TMP_BUFFER \
    ret->tmp_buffer = (char*) malloc(sizeof(char) * (yyleng + 1)); \
    PARSE_ASSERT(ret->tmp_buffer != NULL); \
    strncpy(ret->tmp_buffer, yytext, yyleng); \
    ret->tmp_buffer[yyleng] = '\0'; \

#define COPY_TO_ARG_BUFFER \
    PARSE_ASSERT(ret->tmp_buffer != NULL) \
    PARSE_ASSERT(ret->argument_buffer = strdup(ret->tmp_buffer)); \
    free(ret->tmp_buffer); \
    ret->tmp_buffer = NULL;

static int __mse_handle_set_generator(mse_parser_status_t *ret, int negate)
{
    mse_set_generator_t tmp;
    ASSERT(mse_init_set_generator(&tmp,
                                  ret->parser_gen_type,
                                  ret->parser_op_type,
                                  ret->argument_buffer,
                                  strlen(ret->argument_buffer)));
    tmp.negate = negate;
    ASSERT(ret->set_generator_node = mse_init_interp_node_generator(tmp));
    return 1;
}

/// Calls the handler for a set generator then cleans the internal state
static int mse_handle_set_generator(int negate, mse_parser_status_t *ret)
{
    ASSERT(ret->argument_buffer != NULL);
    int r = __mse_handle_set_generator(ret, negate);

    free(ret->argument_buffer);
    ret->argument_buffer = NULL;
    return r;
}

static int __mse_insert_node_special(mse_parser_status_t *state, 
                                     mse_interp_node_t *node)
{
    if (state->node->r == NULL) {
        state->node->r = node;
        state->node = node;
        return 1;
    }

    // Rotates the tree
    //   snode
    // l       r
    // To
    //           node
    //     snode       r
    // l
    mse_interp_node_t tmp = *state->node;
    *state->node = *node;
    state->node->l = node;
    state->node->r = tmp.r;

    *node = tmp;
    node->r = NULL;
    
    state->node = node;
    return 1;
}

static int __mse_insert_node(mse_parser_status_t *state, mse_interp_node_t *node)
{
    ASSERT(node != NULL);
    if (state->root == NULL) {
        state->root = state->node = node;
        return 1;
    }

    if (state->node->l == NULL) {
        state->node->l = node;
        return 1;
    } else if (state->node->r == NULL) {
        state->node->r = node;
        return 1;
    } else {
        return __mse_insert_node_special(state, node);
    }
}

static int __mse_parser_status_push(mse_parser_status_t *state)
{
    ASSERT(state->stack_roots = realloc(state->stack_roots,
                                        ++state->stack_roots_len));
    state->stack_roots[state->stack_roots_len - 1] = state->node;
    return 1;
}

static int __mse_parser_status_pop(mse_parser_status_t *state)
{
    ASSERT(state->stack_roots_len > 0);
    mse_interp_node_t *tmp = state->stack_roots[state->stack_roots_len - 1];
    if (state->stack_roots_len - 1 == 0) {
        free(state->stack_roots);
        state->stack_roots = NULL;
    } else {
        ASSERT(state->stack_roots = realloc(state->stack_roots,
                                            state->stack_roots_len - 1));
    }
    state->stack_roots_len--;

    if (tmp == NULL) {
        tmp = state->root;
    }
    state->node = tmp;
    return 1;
}

static int __mse_negate(mse_parser_status_t *state)
{
    mse_set_consumer_t consumer;
    ASSERT(mse_init_set_consumer(&consumer,
                                 MSE_SET_CONSUMER_NEGATE,
                                 "",
                                 0));

    mse_interp_node_t *node = NULL;
    ASSERT(node = mse_init_interp_node_consumer(consumer));
    if (state->root == NULL) {
        state->root = state->node = node;
    } else {
        if (state->node->l == NULL) {
             state->node->l = node;
        } else {
            // I can promise that this is not as sketchy as it looks
            state->node->l->l = node;
        }
        state->node = node;
    }
    return 1;
}

%}

// Token match definitions
%%
input: query
     | %empty
     ;

op_operator : LT_INC { ret->parser_op_type = MSE_SET_GENERATOR_OP_LT_INC; }
            | LT { ret->parser_op_type = MSE_SET_GENERATOR_OP_LT; }
            | GT { ret->parser_op_type = MSE_SET_GENERATOR_OP_GT; }
            | GT_INC { ret->parser_op_type = MSE_SET_GENERATOR_OP_GT_INC; }
            | INCLUDES { ret->parser_op_type = MSE_SET_GENERATOR_OP_INCLUDES; }
            | EQUALS { ret->parser_op_type = MSE_SET_GENERATOR_OP_EQUALS; }
            ;

word: WORD { COPY_TO_TMP_BUFFER }
    ;

string: STRING { COPY_TO_TMP_BUFFER }
      ;

regex_string: REGEX_STRING { COPY_TO_TMP_BUFFER }
            ;

op_name: word {
           PARSE_ASSERT(mse_gen_type(ret->tmp_buffer, &ret->parser_gen_type));
       }
       ;

op_argument: string { COPY_TO_ARG_BUFFER }
           | regex_string { COPY_TO_ARG_BUFFER }
           | word { COPY_TO_ARG_BUFFER }
           ;

sg_dummy: STMT_NEGATE op_name op_operator op_argument {
            PARSE_ASSERT(mse_handle_set_generator(1, ret)); 
        }

        | op_name op_operator op_argument {
            PARSE_ASSERT(mse_handle_set_generator(0, ret)); 
        }

        | word {
            COPY_TO_ARG_BUFFER
            ret->parser_gen_type = MSE_SET_GENERATOR_NAME;
            ret->parser_op_type = MSE_SET_GENERATOR_OP_EQUALS;
            PARSE_ASSERT(mse_handle_set_generator(0, ret));
        }

        | string {
            COPY_TO_ARG_BUFFER
            ret->parser_gen_type = MSE_SET_GENERATOR_NAME;
            ret->parser_op_type = MSE_SET_GENERATOR_OP_EQUALS;
            PARSE_ASSERT(mse_handle_set_generator(0, ret));
        }

        | regex_string {
            COPY_TO_ARG_BUFFER
            ret->parser_gen_type = MSE_SET_GENERATOR_NAME;
            ret->parser_op_type = MSE_SET_GENERATOR_OP_EQUALS;
            PARSE_ASSERT(mse_handle_set_generator(0, ret));
        }
        ;

set_generator : sg_dummy {
                   PARSE_ASSERT(__mse_insert_node(ret, ret->set_generator_node));
                   ret->set_generator_node = NULL;
              } %dprec 3

              | {
                  PARSE_ASSERT(__mse_parser_status_push(ret));
              } OPEN_BRACKET query CLOSE_BRACKET { 
                  PARSE_ASSERT(__mse_parser_status_pop(ret));
              } %dprec 2

              | STMT_NEGATE {
                  PARSE_ASSERT(__mse_negate(ret));
                  PARSE_ASSERT(__mse_parser_status_push(ret));
              } OPEN_BRACKET query CLOSE_BRACKET {
                  PARSE_ASSERT(__mse_parser_status_pop(ret));
              } %dprec 1
              ;

operator : AND {
             PARSE_ASSERT(ret->op_node = mse_init_interp_node_operation(MSE_SET_INTERSECTION));
         }
         | OR {
             PARSE_ASSERT(ret->op_node = mse_init_interp_node_operation(MSE_SET_UNION));
         }
         ;

query: set_generator %dprec 5
     
     | query WHITESPACE operator WHITESPACE {
         PARSE_ASSERT(__mse_insert_node(ret, ret->op_node));
         ret->op_node = NULL;
     } set_generator %dprec 1

     | query WHITESPACE {
         PARSE_ASSERT(ret->op_node = mse_init_interp_node_operation(MSE_SET_INTERSECTION));
         PARSE_ASSERT(__mse_insert_node(ret, ret->op_node));
         ret->op_node = NULL;
     } set_generator %dprec 2
     ;
%%

static void __mse_free_parser_status(mse_parser_status_t *status)
{
    if (status->tmp_buffer != NULL) {
        free(status->tmp_buffer);
    }
    if (status->argument_buffer != NULL) {
        free(status->argument_buffer);
    }

    if (status->stack_roots != NULL) {
        for (size_t i = 0; i < status->stack_roots_len; i++) {
            lprintf(LOG_WARNING, "Unused nodes on the stack\n");
            mse_free_interp_node(status->stack_roots[i]);
        }
        free(status->stack_roots);
    }
}

int mse_parse_input_string(const char* input_string, mse_interp_node_t **root)
{
    *root = NULL;

    mse_parser_status_t ret;
    memset(&ret, 0, sizeof(ret));

    pthread_mutex_lock(&parser_lock);

    YY_BUFFER_STATE input_buffer = yy_scan_string(input_string);
    int result = yyparse(&ret);
    yy_delete_buffer(input_buffer);

    pthread_mutex_unlock(&parser_lock);

    // Cleanup
    __mse_free_parser_status(&ret);

    *root = ret.root;
    ASSERT(root != NULL);
    return result == 0;
}
