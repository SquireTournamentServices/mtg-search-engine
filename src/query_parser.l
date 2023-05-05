%option noyywrap noinput nounput

%{
#include "mse_query_parser.h"
%}

%%
[a-zA-Z0-9]+ { return WORD; }
\"([^"]|(\\\"))+\" { return STRING; }
\/([^/]|(\\\/))+\/ { return REGEX_STRING; }

[aA][nN][dD] { return AND; }
[oO][rR] { return OR; }

[\t ]+ { return WHITESPACE; }

"<" { return LT; }
"<=" { return LT_INC; }
">" { return GT; }
">=" { return GT_INC; }
":" { return INCLUDES; }
"=" { return EQUALS; }
"(" { return OPEN_BRACKET; }
")" { return CLOSE_BRACKET; }

%%
