%option noyywrap noinput nounput

%{
#include "query_parser.h"

%}

%%
"[a-zA-Z0-9]+" { return WORD; }
"\"([^\"]|(\\\\\"))+\"" { return STRING; }

"\\/([^/]|(\\\\\\\\\\/))+\\/" { return REGEX_STRING; }

"[aA][nN][dD]" { return AND; }
"[oO][rR]" { return OR; }

"\\s+" { return WHITESPACE; }
%%
