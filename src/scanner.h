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

#ifndef SCANNER_H
#define SCANNER_H

/**
 * Generated Flex class name is yyFlexLexer by default. If we want to use more
 * flex-generated classes we should name them differently. See scanner.l prefix
 * option.
 *
 * Unfortunately the implementation relies on this trick with redefining class
 * name with a preprocessor macro. See GNU Flex manual, "Generating C++
 * Scanners" section
 */
#if !defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer \
    EzAquarii_FlexLexer  // the trick with prefix; no namespace here :(
#include <FlexLexer.h>
#endif

// Scanner method signature is defined by this macro. Original yylex() returns
// int. Sinice Bison 3 uses symbol_type, we must change returned type. We also
// rename it to something sane, since you cannot overload return type.
#undef YY_DECL
#define YY_DECL \
    EzAquarii::Parser::symbol_type EzAquarii::Scanner::get_next_token()

#include <string>

#include "parser.hpp"  // this is needed for symbol_type

namespace EzAquarii {

// Forward declare driver to avoid include. Header is added inimplementation
// file.
class ParserDriver;

class Scanner : public yyFlexLexer {
   public:
    Scanner(ParserDriver& driver) : driver_(driver) {}
    virtual ~Scanner() {}
    virtual EzAquarii::Parser::symbol_type get_next_token();

   private:
    ParserDriver& driver_;
};

}  // namespace EzAquarii

inline std::string unescape(const std::string& input) {
    std::string result;
    for (size_t i = 1; i < input.length() - 1;
         ++i) {  // skip surrounding quotes
        if (input[i] == '\\') {
            ++i;
            if (i >= input.length() - 1) break;
            switch (input[i]) {
                case 'n':
                    result += '\n';
                    break;
                case 't':
                    result += '\t';
                    break;
                case 'r':
                    result += '\r';
                    break;
                case 'b':
                    result += '\b';
                    break;
                case 'f':
                    result += '\f';
                    break;
                case 'v':
                    result += '\v';
                    break;
                case '\\':
                    result += '\\';
                    break;
                case '\'':
                    result += '\'';
                    break;
                case '\"':
                    result += '\"';
                    break;
                case 'x': {
                    std::string hex;
                    while (i + 1 < input.length() - 1 &&
                           isxdigit(input[i + 1])) {
                        hex += input[++i];
                    }
                    if (!hex.empty())
                        result +=
                            static_cast<char>(std::stoi(hex, nullptr, 16));
                    break;
                }
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7': {  // octal
                    std::string oct(1, input[i]);
                    if (i + 1 < input.length() - 1 && isdigit(input[i + 1]))
                        oct += input[++i];
                    if (i + 1 < input.length() - 1 && isdigit(input[i + 1]))
                        oct += input[++i];
                    result += static_cast<char>(std::stoi(oct, nullptr, 8));
                    break;
                }
                default:
                    result += input[i];  // unknown escape, just add it
            }
        } else {
            result += input[i];
        }
    }
    return result;
}

#endif