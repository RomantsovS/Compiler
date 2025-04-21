%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0"
%defines
%define api.parser.class { Parser }

%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define api.namespace { EzAquarii }
%code requires
{
    #include <iostream>
    #include <string>
    #include <vector>
    #include <stdint.h>
    #include "ast.h"
    #include "block.h"
    #include "name_type.h"

    using namespace std;

    namespace EzAquarii {
        class Scanner;
        class Interpreter;
    }
}

// Bison calls yylex() function that must be provided by us to suck tokens
// from the scanner. This block will be placed at the beginning of IMPLEMENTATION file (cpp).
// We define this function here (function! not method).
// This function is called only inside Bison, so we make it static to limit symbol visibility for the linker
// to avoid potential linking conflicts.
%code top
{
    #include <iostream>
    #include "scanner.h"
    #include "parser.hpp"
    #include "interpreter.h"
    #include "location.hh"
    
    // yylex() arguments are defined in parser.y
    static EzAquarii::Parser::symbol_type yylex(EzAquarii::Scanner &scanner) {
        return scanner.get_next_token();
    }
    
    // you can accomplish the same thing by inlining the code using preprocessor
    // x and y are same as in above static function
    // #define yylex(x, y) scanner.get_next_token()
    
    using namespace EzAquarii;
}

%lex-param { EzAquarii::Scanner &scanner }

%parse-param { EzAquarii::Scanner &scanner }
%parse-param { EzAquarii::Interpreter &driver }
%parse-param { std::shared_ptr<ASTNode>& result }

%locations
%define parse.trace
%define parse.error verbose

%define api.token.prefix {TOKEN_}

%token END 0 "end of file"
%token <uint64_t> NUMBER "number";
%token LEFTPAR "leftpar";
%token RIGHTPAR "rightpar";
%token LEFTFIGPAR;
%token RIGHTFIGPAR;
%token SEMICOLON "semicolon";
%token COMMA "comma";
%token EQUAL;
%token PLUS MINUS;
%token MULTIPLY DIVIDE;
%token LESS GREATER;
%token IF ELSE;
%token WHILE;
%token <std::string> ID
%token INT
%token PRINT
%token RETURN

%type< Type > type;
%type< std::shared_ptr<Block> > block;
%type< std::shared_ptr<Statements> > stmt_list function_list arg_list;
%type< std::shared_ptr<Params> > param_list;
%type< std::shared_ptr<ASTNode> > program function function_call declaration expr assignment print_stmt stmt while if;

%start program

%%

program:
    function_list {
        result = make_program($1);
    }
    ;

function_list:
    function_list function {
        $$ = append_function($1, $2);
    }
    | /* empty */ {
        $$ = make_empty_function_list();
    }
    ;

function:
    type ID LEFTPAR param_list RIGHTPAR block { 
        $$ = make_function($1, $2, $4, $6);
    }
    ;

type:
    INT {
        $$ = Type::Int;
    }
    ;

param_list:
    /* empty */ {
        $$ = make_empty_param_list();
    }
    | type ID {
        $$ = make_param_list($1, $2);
    }
    | param_list COMMA type ID {
        $$ = append_param($1, $3, $4);
    }
    ;

function_call:
    ID LEFTPAR arg_list RIGHTPAR { 
        $$ = make_function_call($1, $3);
    }
    ;

arg_list:
    /* empty */ {
        $$ = make_empty_arg_list();
    }
    | expr {
        $$ = make_arg_list($1);
    }
    | arg_list COMMA expr {
        $$ = append_arg($1, $3);
    }
    ;

block:
    LEFTFIGPAR stmt_list RIGHTFIGPAR {
        $$ = make_block($2);
    }
    ;

stmt_list:
    stmt_list stmt {
        $$ = append_stmt($1, $2);
    }
    | /* empty */ {
        $$ = make_empty_stmt_list();
    }
    ;

while:
    WHILE LEFTPAR expr RIGHTPAR stmt {
        $$ = make_while($3, $5);
    }

if:
    IF LEFTPAR expr RIGHTPAR stmt {
        $$ = make_if($3, $5, nullptr);
    }
    | IF LEFTPAR expr RIGHTPAR stmt ELSE stmt {
        $$ = make_if($3, $5, $7);
    }

stmt:
    declaration
    | assignment
    | while
    | if
    | print_stmt
    | RETURN expr SEMICOLON {
        $$ = make_return($2);
    }
    ;

declaration:
    type ID SEMICOLON {
		$$ = make_decl($1, $2);
    }
    ;

assignment:
    ID EQUAL expr SEMICOLON {
        $$ = make_assignment($1, $3);
    }
    ;

print_stmt:
    PRINT LEFTPAR expr RIGHTPAR SEMICOLON {
        $$ = make_print($3);
    }
    ;

expr:
    LEFTPAR expr RIGHTPAR { $$ = $2; }
    | function_call { $$ = $1; }
    | expr PLUS expr { $$ = make_arith_op("+", $1, $3); }
    | expr MINUS expr { $$ = make_arith_op("-", $1, $3); }
    | expr MULTIPLY expr { $$ = make_arith_op("*", $1, $3); }
    | expr DIVIDE expr { $$ = make_arith_op("/", $1, $3); }
    | expr LESS expr { $$ = make_logic_op("<", $1, $3); }
    | expr GREATER expr { $$ = make_logic_op(">", $1, $3); }
    | ID { $$ = make_var($1); }
    | NUMBER { $$ = make_integer($1); }
    ;

%%

// Bison expects us to provide implementation - otherwise linker complains
void EzAquarii::Parser::error([[maybe_unused]] const location &loc , const std::string &message) {	
    cout << "Error: " << message << endl << "Error location: " << driver.location() << endl;
}
