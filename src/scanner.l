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
	#include "interpreter.h"
	#include "parser.hpp"
	#include "location.hh"

	using namespace std;

	// Original yyterminate() macro returns int. Since we're using Bison 3 variants
	// as tokens, we must redefine it to change type from `int` to `Parser::semantic_type`
	#define yyterminate() EzAquarii::Parser::make_END(EzAquarii::location());

	// This will track current scanner location.
	// Action is called when length of the token is known.
	#define YY_USER_ACTION driver_.increaseLocation(yyleng);
	
	// !!!WARNING!!!
	// Location API is used, but the location is not initialized, 'cause I'm lazy. When making
	// a token with make_{something} method you can pass detailed token location. Current location
	// is accessible with m_driver.location() method. All puzzle elements are there - just
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

"int"       {
                cout << "Scanner: int " << std::endl;
                return EzAquarii::Parser::make_INT(EzAquarii::location( /* put location data here if you want */ )); 
            }

"print"     {
                cout << "Scanner: print " << std::endl;
                return EzAquarii::Parser::make_PRINT(EzAquarii::location( /* put location data here if you want */ )); 
            }

"if"        {
                cout << "Scanner: 'if'" << endl;
                return EzAquarii::Parser::make_IF(EzAquarii::location());
            }

"else"      {
                cout << "Scanner: 'else'" << endl;
                return EzAquarii::Parser::make_ELSE(EzAquarii::location());
            }

"while"     {
                cout << "Scanner: 'while'" << endl;
                return EzAquarii::Parser::make_WHILE(EzAquarii::location());
            }

"return"    {
                cout << "Scanner: 'return'" << endl;
                return EzAquarii::Parser::make_RETURN(EzAquarii::location());
            }

\"([^\\"]|\\.)*\" {
                cout << "Scanner: 'string'" << endl;
                std::string raw_str(yytext);
                raw_str = raw_str.substr(1, raw_str.length() - 2); // Strip quotes
                return EzAquarii::Parser::make_STRING(raw_str, EzAquarii::location());
                }

[a-zA-Z_][a-zA-Z0-9_]*  {
						cout << "Scanner: identifier [" << yytext << "]" << endl;
                        return EzAquarii::Parser::make_ID(yytext, EzAquarii::location( /* put location data here if you want */ )); 
                        }
            
\(          {
                cout << "Scanner: '('" << endl;
                return EzAquarii::Parser::make_LEFTPAR(EzAquarii::location());
            }
            
\)          { 
                cout << "Scanner: ')'" << endl;
                return EzAquarii::Parser::make_RIGHTPAR(EzAquarii::location());
            }

\{          {
                cout << "Scanner: '{'" << endl;
                return EzAquarii::Parser::make_LEFTFIGPAR(EzAquarii::location());
            }
            
\}          { 
                cout << "Scanner: '}'" << endl;
                return EzAquarii::Parser::make_RIGHTFIGPAR(EzAquarii::location());
            }
            
;           {
                cout << "Scanner: ';'" << endl;
                return EzAquarii::Parser::make_SEMICOLON(EzAquarii::location());
            }
            
,           {
                cout << "Scanner: ','" << endl;
                return EzAquarii::Parser::make_COMMA(EzAquarii::location());
            }

=           {
                cout << "Scanner: '='" << endl;
                return EzAquarii::Parser::make_EQUAL(EzAquarii::location());
            }

"+"         {
                cout << "Scanner: '+'" << endl;
                return EzAquarii::Parser::make_PLUS(EzAquarii::location());
            }

"-"         {
                cout << "Scanner: '-'" << endl;
                return EzAquarii::Parser::make_MINUS(EzAquarii::location());
            }

"*"         {
                cout << "Scanner: '*'" << endl;
                return EzAquarii::Parser::make_MULTIPLY(EzAquarii::location());
            }

"/"         {
                cout << "Scanner: '/'" << endl;
                return EzAquarii::Parser::make_DIVIDE(EzAquarii::location());
            }

"<"         {
                cout << "Scanner: '<'" << endl;
                return EzAquarii::Parser::make_LESS(EzAquarii::location());
            }

">"         {
                cout << "Scanner: '<'" << endl;
                return EzAquarii::Parser::make_GREATER(EzAquarii::location());
            }
            
[\n\t ]     {
                //cout << "Scanner: whitechar (ignored)" << endl;
            }

[0-9]+ {
                cout << "Scanner: decimal number: " << yytext << endl;
                uint64_t number = strtoull(yytext, 0, 10);
                return EzAquarii::Parser::make_NUMBER(number, EzAquarii::location());
            }

.           { 
                cout << "Scanner: unknown character [" << yytext << "]" << endl; 
            }
            
<<EOF>>     { return yyterminate(); }


%%
