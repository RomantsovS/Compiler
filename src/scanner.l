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

%{
	#include <iostream>
	#include <cstdlib>
	#include "scanner.h"
	#include "parser_driver.h"
	#include "parser.hpp"
	#include "location.hh"

	// Original yyterminate() macro returns int. Since we're using Bison 3 variants
	// as tokens, we must redefine it to change type from `int` to `Parser::semantic_type`
	#define yyterminate() EzAquarii::Parser::make_END(driver_.get_location());

	// This will track current scanner location.
	// Action is called when length of the token is known.
	#define YY_USER_ACTION driver_.ScannerAction(yyleng, yytext);
	
	// !!!WARNING!!!
	// Location API is used, but the location is not initialized, 'cause I'm lazy. When making
	// a token with make_{something} method you can pass detailed token location. Current location
	// is accessible with m_driver.get_location() method. All puzzle elements are there - just
	// pass location value in every action code block below. I'm going to waste more time writing
	// this excuse than putting this boilerplate below...
	//
	// Location class can be found in location.hh and posistion.hh files. It's just a bit too much
	// boilerplate for this small example. Bummer.
%}

%option nodefault
%option noyywrap
%option c++
%option yyclass="Scanner"
%option prefix="EzAquarii_"

%%

"void"      {
                return EzAquarii::Parser::make_VOID(driver_.get_location()); 
            }

"int"       {
                return EzAquarii::Parser::make_INT(driver_.get_location()); 
            }

"bool"      {
                return EzAquarii::Parser::make_BOOL(driver_.get_location()); 
            }

"print"     {
                return EzAquarii::Parser::make_PRINT(driver_.get_location()); 
            }

"if"        {
                return EzAquarii::Parser::make_IF(driver_.get_location());
            }

"else"      {
                return EzAquarii::Parser::make_ELSE(driver_.get_location());
            }

"while"     {
                return EzAquarii::Parser::make_WHILE(driver_.get_location());
            }

"return"    {
                return EzAquarii::Parser::make_RETURN(driver_.get_location());
            }

"true"      {
                return EzAquarii::Parser::make_TRUE(true, driver_.get_location());
            }

"false"     {
                return EzAquarii::Parser::make_FALSE(false, driver_.get_location());
            }

"//".*      { /* skip single-line comment */ }

\"([^\\\n"]|\\[abfnrtv\\'\"0-7xX])*?\" {
                std::string raw_str(yytext);
                return EzAquarii::Parser::make_STRING(unescape(raw_str), driver_.get_location());
                }

[a-zA-Z_][a-zA-Z0-9_]*  {
                        return EzAquarii::Parser::make_ID(yytext, driver_.get_location()); 
                        }
            
\(          {
                return EzAquarii::Parser::make_LEFTPAR(driver_.get_location());
            }
            
\)          { 
                return EzAquarii::Parser::make_RIGHTPAR(driver_.get_location());
            }

\{          {
                return EzAquarii::Parser::make_LEFTFIGPAR(driver_.get_location());
            }
            
\}          { 
                return EzAquarii::Parser::make_RIGHTFIGPAR(driver_.get_location());
            }

"["         {
                return EzAquarii::Parser::make_LEFTBRACKET(driver_.get_location());
            }
            
"]"         {
                return EzAquarii::Parser::make_RIGHTBRACKET(driver_.get_location());
            }
            
;           {
                return EzAquarii::Parser::make_SEMICOLON(driver_.get_location());
            }
            
,           {
                return EzAquarii::Parser::make_COMMA(driver_.get_location());
            }

=           {
                return EzAquarii::Parser::make_EQUAL(driver_.get_location());
            }

"+"         {
                return EzAquarii::Parser::make_PLUS(driver_.get_location());
            }

"-"         {
                return EzAquarii::Parser::make_MINUS(driver_.get_location());
            }

"*"         {
                return EzAquarii::Parser::make_MULTIPLY(driver_.get_location());
            }

"/"         {
                return EzAquarii::Parser::make_DIVIDE(driver_.get_location());
            }

"%"         {
                return EzAquarii::Parser::make_MOD(driver_.get_location());
            }

"<"         {
                return EzAquarii::Parser::make_LESS(driver_.get_location());
            }

">"         {
                return EzAquarii::Parser::make_GREATER(driver_.get_location());
            }
            
[\t ]       {
            }

[\n]        {
                driver_.newline();
            }

[0-9]+      {
                uint64_t number = strtoull(yytext, 0, 10);
                return EzAquarii::Parser::make_NUMBER(number, driver_.get_location());
            }

.           { 
                cout << "Scanner: unknown character [" << yytext << "]" << endl; 
            }
            
<<EOF>>     { return yyterminate(); }


%%
