%{
#include "ast.h"
#include <string>

extern int yylex();
extern int yyparse(std::shared_ptr<ASTNode>* root);
void yyerror(std::shared_ptr<ASTNode>* root, const char *s);

// symbol table (very basic)
int symbol_table[256];  // assuming one-letter variable names like x, y, z
%}

%code requires {
  #include "ast.h"
}

%union {
    int ival;
    std::string* sval;
	std::shared_ptr<ASTNode>* node;
}

%parse-param { std::shared_ptr<ASTNode>* root }

%token <sval> ID
%token <ival> NUMBER
%token PRINT
%token INT

%type <ival> expr
%type <node> declaration program function

%%

program:
    function {
        root = $1;
	}
    ;

function:
    INT ID '(' ')' block { 
        $$ = new std::shared_ptr<ASTNode>(make_function(*$2));
        delete $2;
    }
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
		$$ = new std::shared_ptr<ASTNode>(make_decl(*$2));
    }
    ;

assignment:
    ID '=' expr ';' {
        symbol_table[(*$1)[0]] = $3;
        free($1);
    }
    ;

print_stmt:
    PRINT '(' ID ')' ';' {
        printf("%d\n", symbol_table[(*$3)[0]]);
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
    | ID            { $$ = symbol_table[(*$1)[0]]; free($1); }
    ;

%%

int main() {
    std::shared_ptr<ASTNode>* root = nullptr;
    yyparse(root);  // or assign root inside grammar
    //print_ast(root, 0);
    //free_ast(root);
    return 0;
}

void yyerror(std::shared_ptr<ASTNode>* root, const char *s) {
    fprintf(stderr, "Parser error: %s\n", s);
}
