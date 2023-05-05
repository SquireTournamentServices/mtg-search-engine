%option noyywrap noinput nounput

%{
#include "mse_query_parser.h"
%}

SLASH \/
QUOTE \"

%%
[a-zA-Z0-9]+ { return WORD; }
QUOTE([^\"]|(\\\\QUOTE))+QUOTE { return STRING; }

SLASH([^/]|(\\\\SLASH))+SLASH { return REGEX_STRING; }

[aA][nN][dD] { return AND; }
[oO][rR] { return OR; }

\\s+ { return WHITESPACE; }

"<" { return LT; }
"<=" { return LT_INC; }
">" { return GT; }
">=" { return GT_INC; }
":" { return INCLUDES; }
"=" { return EQUALS; }

%%
