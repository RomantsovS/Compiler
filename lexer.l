%option noyywrap

%{
#include "parser.hpp"
#include <string>
%}

%%

"int"           { printf("TOKEN: INT\n"); return INT; }
"print"         { printf("TOKEN: PRINT\n"); return PRINT; }
 
[0-9]+          { printf("TOKEN: NUMBER (%s)\n", yytext); yylval.ival = atoi(yytext); return NUMBER; }

[a-zA-Z_][a-zA-Z0-9_]*  {
						printf("TOKEN: ID (%s)\n", yytext);
                        yylval.sval = new std::string(yytext);
                        return ID;
                      }

"=="            { return '='; }  // optional: for later
"="             { printf("TOKEN: =\n"); return '='; }
";"             { return ';'; }
"("             { return '('; }
")"             { return ')'; }
"{"             { return '{'; }
"}"             { return '}'; }
"+"             { printf("TOKEN: +\n"); return '+'; }
"-"             { return '-'; }
"*"             { printf("TOKEN: *\n"); return '*'; }
"/"             { return '/'; }

[ \t\r\n]+      ;  // skip whitespace

.               { return yytext[0]; }

%%
