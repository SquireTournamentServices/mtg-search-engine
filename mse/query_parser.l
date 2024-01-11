%option bison-bridge bison-locations yylineno 
%option nodefault reentrant noinput nounput

%{
#include "mse/query_parser.h"
#include "mse_query_lexer.h"
#include "mse_query_parser.h"

void yyerror(YYLTYPE* loc, mse_parser_status_t *data, const char *msg);

#define YY_USER_ACTIO { yylloc->first_line = yylloc->last_line; yylloc->first_column = yylloc->last_column; for(int i = 0; yytext[i] != '\0'; i++) { if(yytext[i] == '\n') { yylloc->last_line++; yylloc->last_column = 0; } else { yylloc->last_column++; } } } 
%}

%%
"and" { return AND; }
"or" { return OR; }

[ \t]+ { return WHITESPACE; }

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

void yyerror(YYLTYPE* loc, mse_parser_status_t *data, const char *s)
{
    fprintf(stderr, "Lexer error at line %d: %s\n", loc->first_line, s);
}
