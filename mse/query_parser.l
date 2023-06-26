%option noyywrap noinput nounput

%{
#include "mse_query_parser.h"
%}

%%
[aA][nN][dD] { return AND; }
[oO][rR] { return OR; }

\s+ { return WHITESPACE; }

"<" { return LT; }
"<=" { return LT_INC; }
">" { return GT; }
">=" { return GT_INC; }
":" { return INCLUDES; }
"=" { return EQUALS; }
"(" { return OPEN_BRACKET; }
")" { return CLOSE_BRACKET; }
"-" { return STMT_NEGATE; }

[a-zA-Z0-9_]+ { return WORD; }
\"([^"]|(\\\"))+\" { return STRING; }
\/([^/]|(\\\/))+\/ { return REGEX_STRING; }
%%
