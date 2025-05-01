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
    #include "ast/ast.h"
    #include "ast/type.h"

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
%parse-param { std::shared_ptr<AST::ASTNode>& result }

%locations
%define parse.trace
%define parse.error verbose

%define api.token.prefix {TOKEN_}

%token END 0 "end of file"
%token <uint64_t> NUMBER;
%token LEFTPAR RIGHTPAR;
%token LEFTFIGPAR RIGHTFIGPAR;
%token LEFTBRACKET RIGHTBRACKET;
%token SEMICOLON;
%token COMMA;
%token EQUAL;
%token PLUS MINUS MULTIPLY DIVIDE MOD;
%token LESS GREATER;
%token IF ELSE;
%token WHILE;
%token <std::string> ID
%token INT VOID
%token PRINT
%token RETURN
%token <std::string> STRING
%token <bool> BOOL

%type< AST::Type > type;
%type< std::shared_ptr<AST::Statements> > stmt_list block top_level_list arg_list;
%type< std::shared_ptr<AST::Params> > param_list;
%type< std::shared_ptr<AST::ASTNode> > program top_level global function function_call print_stmt;
%type< std::shared_ptr<AST::ASTNode> > declaration expr assignment literal stmt while if;

%start program

%%

program:
    top_level_list {
        result = AST::make_program($1);
    }
    ;

top_level:
    global
    | function
    ;

top_level_list:
    top_level_list top_level {
        $$ = AST::append_top_level($1, $2);
    }
    | /* empty */ {
        $$ = AST::make_empty_top_level_list();
    }
    ;

global:
    declaration
    ;

function:
    type ID LEFTPAR param_list RIGHTPAR block { 
        $$ = AST::make_function($1, $2, $4, $6);
    }
    ;

type:
    INT {
        $$ = AST::Type::Int();
    }
    | VOID {
        $$ = AST::Type::Void();
    }
    | INT LEFTBRACKET NUMBER RIGHTBRACKET {
        $$ = AST::Type::IntArray($3);
    } 
    ;

param_list:
    /* empty */ {
        $$ = AST::make_empty_param_list();
    }
    | type ID {
        $$ = AST::make_param_list($1, $2);
    }
    | param_list COMMA type ID {
        $$ = AST::append_param($1, $3, $4);
    }
    ;

function_call:
    ID LEFTPAR arg_list RIGHTPAR { 
        $$ = AST::make_function_call($1, $3);
    }
    ;

arg_list:
    /* empty */ {
        $$ = AST::make_empty_arg_list();
    }
    | expr {
        $$ = AST::make_arg_list($1);
    }
    | arg_list COMMA expr {
        $$ = AST::append_arg($1, $3);
    }
    ;

block:
    LEFTFIGPAR stmt_list RIGHTFIGPAR {
        $$ = $2;
    }
    ;

stmt_list:
    stmt_list stmt {
        $$ = AST::append_stmt($1, $2);
    }
    | /* empty */ {
        $$ = AST::make_empty_stmt_list();
    }
    ;

while:
    WHILE LEFTPAR expr RIGHTPAR stmt {
        $$ = AST::make_while($3, $5);
    }
    | WHILE LEFTPAR expr RIGHTPAR block {
        $$ = AST::make_while($3, $5);
    }
    ;

if:
    IF LEFTPAR expr RIGHTPAR stmt {
        $$ = AST::make_if($3, $5, nullptr);
    }
    | IF LEFTPAR expr RIGHTPAR stmt ELSE stmt {
        $$ = AST::make_if($3, $5, $7);
    }
    ;

stmt:
    declaration
    | assignment
    | expr SEMICOLON
    | while
    | if
    | print_stmt
    | RETURN expr SEMICOLON {
        $$ = AST::make_return($2);
    }
    | ID LEFTBRACKET expr RIGHTBRACKET EQUAL expr SEMICOLON {
        $$ = AST::make_array_assignment($1, $3, $6);
    }
    ;

declaration:
    type ID SEMICOLON {
		$$ = AST::make_decl($1, $2);
    }
    | type ID LEFTBRACKET NUMBER RIGHTBRACKET SEMICOLON {
        $$ = with_location(AST::make_array_declaration($2, AST::Type::IntArray($4)), @1);
    }
    ;

assignment:
    ID EQUAL expr SEMICOLON {
        $$ = AST::make_assignment($1, $3);
    }
    ;

print_stmt:
    PRINT LEFTPAR expr RIGHTPAR SEMICOLON {
        $$ = AST::make_print($3);
    }
    ;

literal:
    STRING {
        $$ = AST::make_string_literal($1);
    }
    | BOOL {
        $$ = AST::make_bool_literal($1);
    }
    | NUMBER {
        $$ = AST::make_integer($1);
    }
    ;

expr:
    LEFTPAR expr RIGHTPAR { $$ = $2; }
    | function_call { $$ = $1; }
    | expr PLUS expr { $$ = AST::make_arith_op("+", $1, $3); }
    | expr MINUS expr { $$ = AST::make_arith_op("-", $1, $3); }
    | expr MULTIPLY expr { $$ = AST::make_arith_op("*", $1, $3); }
    | expr DIVIDE expr { $$ = AST::make_arith_op("/", $1, $3); }
    | expr MOD expr { $$ = AST::make_arith_op("%", $1, $3); }
    | expr LESS expr { $$ = AST::make_logic_op("<", $1, $3); }
    | expr GREATER expr { $$ = AST::make_logic_op(">", $1, $3); }
    | ID { $$ = AST::make_var($1); }
    | ID LEFTBRACKET expr RIGHTBRACKET { $$ = AST::make_array_access($1, $3); }
    | literal
    ;

%%

// Bison expects us to provide implementation - otherwise linker complains
void EzAquarii::Parser::error([[maybe_unused]] const location &loc , const std::string &message) {	
    cout << "Error: " << message << endl << "Error location: " << driver.location() << endl;
}
