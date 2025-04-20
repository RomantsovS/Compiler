/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2014 Krzysztof Narkiewicz <krzysztof.narkiewicz@ezaquarii.com>
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 */

%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0"
%defines
%define parser_class_name { Parser }

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
    static EzAquarii::Parser::symbol_type yylex(EzAquarii::Scanner &scanner, EzAquarii::Interpreter &driver) {
        return scanner.get_next_token();
    }
    
    // you can accomplish the same thing by inlining the code using preprocessor
    // x and y are same as in above static function
    // #define yylex(x, y) scanner.get_next_token()
    
    using namespace EzAquarii;
}

%lex-param { EzAquarii::Scanner &scanner }
%lex-param { EzAquarii::Interpreter &driver }
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
%token <std::string> ID
%token INT
%token PRINT

%type< Type > type;
%type< std::shared_ptr<Block> > block;
%type< std::shared_ptr<std::vector<std::shared_ptr<ASTNode>>> > stmt_list;
%type< std::shared_ptr<ASTNode> > function declaration expr assignment print_stmt stmt;

%start program

%%

program:
    function {
        result = $1;
	}
    ;

function:
    type ID LEFTPAR RIGHTPAR block { 
        $$ = make_function($1, $2, $5);
    }
    ;

type:
    INT {
        $$ = Type::Int;
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
    | /* empty */
    ;

stmt:
    declaration
    | assignment
    | print_stmt
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
    expr PLUS expr { $$ = make_arith_op("+", $1, $3); }
    | expr MINUS expr { $$ = make_arith_op("-", $1, $3); }
    | expr MULTIPLY expr { $$ = make_arith_op("*", $1, $3); }
    | expr DIVIDE expr { $$ = make_arith_op("/", $1, $3); }
    | ID { $$ = make_var($1); }
    | NUMBER { $$ = make_integer($1); }
    ;

%%

// Bison expects us to provide implementation - otherwise linker complains
void EzAquarii::Parser::error(const location &loc , const std::string &message) {
        
        // Location should be initialized inside scanner action, but is not in this example.
        // Let's grab location directly from driver class.
	// cout << "Error: " << message << endl << "Location: " << loc << endl;
	
        cout << "Error: " << message << endl << "Error location: " << driver.location() << endl;
}
