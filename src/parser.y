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
    #include "ast/name_type.h"

    using namespace std;

    namespace EzAquarii {
        class Scanner;
        class ParserDriver;
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
    #include "parser_driver.h"
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
%parse-param { EzAquarii::ParserDriver &driver }
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
%token EQUAL ASSIGN;
%token PLUS MINUS MULTIPLY DIVIDE MOD;
%token LESS GREATER;
%token IF ELSE;
%token WHILE;
%token <std::string> ID
%token VOID INT BOOL
%token PRINT
%token RETURN
%token <std::string> STRING
%token <bool> TRUE FALSE
%token RAND

%left EQUAL
%left LESS GREATER
%left PLUS MINUS
%left MULTIPLY DIVIDE MOD

%type< AST::Type > type;
%type< std::shared_ptr<AST::Statements> > stmt_list block top_level_list arg_list;
%type< std::shared_ptr<AST::Params> > param_list;
%type< std::shared_ptr<AST::ASTNode> > program top_level global function function_call print_stmt rand_stmt;
%type< std::shared_ptr<AST::ASTNode> > declaration expr assignment literal stmt while if;

%start program

%%

program:
    top_level_list {
        result = driver.with_location(driver.make_program($1), @1);
    }
    ;

top_level:
    global
    | function
    ;

top_level_list:
    top_level_list top_level {
        $$ = driver.append_top_level($1, $2);
    }
    | /* empty */ {
        $$ = driver.make_empty_top_level_list();
    }
    ;

global:
    declaration
    ;

function:
    type ID LEFTPAR param_list RIGHTPAR block { 
        $$ = driver.with_location(driver.make_function($1, $2, $4, $6), @1);
    }
    ;

type:
    VOID {
        $$ = AST::Type::Void();
    }
    | INT {
        $$ = AST::Type::Int();
    }
    | BOOL {
        $$ = AST::Type::Bool();
    }
    | INT LEFTBRACKET NUMBER RIGHTBRACKET {
        $$ = AST::Type::IntArray($3);
    } 
    ;

param_list:
    /* empty */ {
        $$ = driver.make_empty_param_list();
    }
    | type ID {
        $$ = driver.make_param_list($1, $2);
    }
    | param_list COMMA type ID {
        $$ = driver.append_param($1, $3, $4);
    }
    ;

function_call:
    ID LEFTPAR arg_list RIGHTPAR { 
        $$ = driver.with_location(driver.make_function_call($1, $3), @1);
    }
    ;

arg_list:
    /* empty */ {
        $$ = driver.make_empty_arg_list();
    }
    | expr {
        $$ = driver.make_arg_list($1);
    }
    | arg_list COMMA expr {
        $$ = driver.append_arg($1, $3);
    }
    ;

block:
    LEFTFIGPAR stmt_list RIGHTFIGPAR {
        $$ = $2;
    }
    ;

stmt_list:
    stmt_list stmt {
        $$ = driver.append_stmt($1, $2);
    }
    | /* empty */ {
        $$ = driver.make_empty_stmt_list();
    }
    ;

while:
    WHILE LEFTPAR expr RIGHTPAR stmt {
        $$ = driver.with_location(driver.make_while($3, $5), @1);
    }
    | WHILE LEFTPAR expr RIGHTPAR block {
        $$ = driver.with_location(driver.make_while($3, $5), @1);
    }
    ;

if:
    IF LEFTPAR expr RIGHTPAR stmt {
        $$ = driver.with_location(driver.make_if($3, $5, nullptr), @1);
    }
    | IF LEFTPAR expr RIGHTPAR stmt ELSE stmt {
        $$ = driver.with_location(driver.make_if($3, $5, $7), @1);
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
        $$ = driver.with_location(driver.make_return($2), @1);
    }
    ;

declaration:
    type ID SEMICOLON {
		$$ = driver.with_location(driver.make_decl($1, $2), @1);
    }
    | type ID LEFTBRACKET NUMBER RIGHTBRACKET SEMICOLON {
        $$ = driver.with_location(driver.make_array_declaration($2, AST::Type::IntArray($4)), @1);
    }
    ;

assignment:
    ID ASSIGN expr SEMICOLON {
        $$ = driver.with_location(driver.make_assignment($1, $3), @2);
    }
    | ID LEFTBRACKET expr RIGHTBRACKET ASSIGN expr SEMICOLON {
        $$ = driver.with_location(driver.make_array_assignment($1, $3, $6), @5);
    }
    ;

print_stmt:
    PRINT LEFTPAR expr RIGHTPAR SEMICOLON {
        $$ = driver.with_location(driver.make_print($3), @1);
    }
    ;

rand_stmt:
    RAND LEFTPAR RIGHTPAR {
        $$ = driver.with_location(driver.make_rand(), @1);
    }
    ;

literal:
    STRING {
        $$ = driver.with_location(driver.make_string_literal($1), @1);
    }
    | TRUE {
        $$ = driver.with_location(driver.make_bool_literal($1), @1);
    }
    | FALSE {
        $$ = driver.with_location(driver.make_bool_literal($1), @1);
    }
    | NUMBER {
        $$ = driver.with_location(driver.make_integer($1), @1);
    }
    ;

expr:
    LEFTPAR expr RIGHTPAR { $$ = $2; }
    | function_call { $$ = $1; }
    | expr PLUS expr { $$ = driver.with_location(driver.make_arith_op("+", $1, $3), @2); }
    | expr MINUS expr { $$ = driver.with_location(driver.make_arith_op("-", $1, $3), @2); }
    | expr MULTIPLY expr { $$ = driver.with_location(driver.make_arith_op("*", $1, $3), @2); }
    | expr DIVIDE expr { $$ = driver.with_location(driver.make_arith_op("/", $1, $3), @2); }
    | expr MOD expr { $$ = driver.with_location(driver.make_arith_op("%", $1, $3), @2); }
    | expr LESS expr { $$ = driver.with_location(driver.make_logic_op("<", $1, $3), @2); }
    | expr GREATER expr { $$ = driver.with_location(driver.make_logic_op(">", $1, $3), @2); }
    | expr EQUAL expr { $$ = driver.with_location(driver.make_logic_op("==", $1, $3), @2); }
    | ID { $$ = driver.with_location(driver.make_var($1), @1); }
    | ID LEFTBRACKET expr RIGHTBRACKET { $$ = driver.with_location(driver.make_array_access($1, $3), @2); }
    | literal
    | rand_stmt
    ;

%%

// Bison expects us to provide implementation - otherwise linker complains
void EzAquarii::Parser::error([[maybe_unused]] const location &loc , const std::string &message) {	
    cout << "Error: " << message << endl << "Error location: " << driver.get_location() << endl;
}
