%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex();
extern int yyparse();
void yyerror(const char *s);

// symbol table (very basic)
int symbol_table[256];  // assuming one-letter variable names like x, y, z
%}

%union {
    int ival;
    char* sval;
}

%token <sval> ID
%token <ival> NUMBER
%token PRINT
%token INT

%type <ival> expr

%%

program:
    function
    ;

function:
    INT ID '(' ')' block
    ;

block:
    '{' stmt_list '}'
    ;

stmt_list:
    stmt_list stmt
    | /* empty */
    ;

stmt:
    declaration
    | assignment
    | print_stmt
    ;

declaration:
    INT ID ';' {
        symbol_table[$2[0]] = 0;
        free($2);
    }
    ;

assignment:
    ID '=' expr ';' {
        symbol_table[$1[0]] = $3;
        free($1);
    }
    ;

print_stmt:
    PRINT '(' ID ')' ';' {
        printf("%d\n", symbol_table[$3[0]]);
        free($3);
    }
    ;

expr:
    expr '+' expr { $$ = $1 + $3; }
    | expr '-' expr { $$ = $1 - $3; }
    | expr '*' expr { $$ = $1 * $3; }
    | expr '/' expr { $$ = $1 / $3; }
    | '(' expr ')'  { $$ = $2; }
    | NUMBER        { $$ = $1; }
    | ID            { $$ = symbol_table[$1[0]]; free($1); }
    ;

%%

int main() {
    return yyparse();
}

void yyerror(const char *s) {
    fprintf(stderr, "Syntax error: %s\n", s);
}
