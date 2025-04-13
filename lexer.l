%{
#include "parser.tab.h"
#include <stdlib.h>
#include <string.h>
%}

%%

"int"           { return INT; }
"print"         { return PRINT; }

[0-9]+          { yylval.ival = atoi(yytext); return NUMBER; }

[a-zA-Z_][a-zA-Z0-9_]*  {
                        yylval.sval = strdup(yytext);
                        return ID;
                      }

"=="            { return '='; }  // optional: for later
"="             { return '='; }
";"             { return ';'; }
"("             { return '('; }
")"             { return ')'; }
"{"             { return '{'; }
"}"             { return '}'; }
"+"             { return '+'; }
"-"             { return '-'; }
"*"             { return '*'; }
"/"             { return '/'; }

[ \t\r\n]+      ;  // skip whitespace

.               { return yytext[0]; }

%%
