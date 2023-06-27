%option noyywrap noinput nounput case-insensitive

%{
#include "mse_query_parser.h"
%}

%%
"and" { return AND; }
"or" { return OR; }

" " { return WHITESPACE; }

"<" { return LT; }
"<=" { return LT_INC; }
">" { return GT; }
">=" { return GT_INC; }
":" { return INCLUDES; }
"=" { return EQUALS; }
"(" { return OPEN_BRACKET; }
")" { return CLOSE_BRACKET; }
"-" { return STMT_NEGATE; }

[a-z0-9_]+ { return WORD; }
\"([^"]|(\\\"))+\" { return STRING; }
\/([^/]|(\\\/))+\/ { return REGEX_STRING; }
%%
