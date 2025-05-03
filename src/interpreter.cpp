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

#include "interpreter.h"

#include <sstream>

#include "ast/ast.h"

using namespace EzAquarii;

Interpreter::Interpreter(std::shared_ptr<AST::ASTNode>& ast)
    : scanner_(*this), parser_(scanner_, *this, ast) {}

int Interpreter::parse() {
    // parser_.set_debug_level(1);
    return parser_.parse();
}

void Interpreter::switchInputStream(std::istream* is) {
    scanner_.switch_streams(is, NULL);
}

void Interpreter::ScannerAction(unsigned int length, std::string_view text) {
    loc_.step();
    loc_.columns(length);
    if (scanner_debug_level > 0) {
        std::cout << "Scanner: " << text << '\n';
        cout << "increaseLocation(): " << loc_ << '\n';
    }
}

void Interpreter::newline() { loc_.lines(1); }

const location& Interpreter::get_location() const { return loc_; }

void Interpreter::SetScannerDebugLevel(int level) {
    scanner_debug_level = level;
}

void Interpreter::SetParserDebugLevel(int level) { parser_debug_level = level; }

void Interpreter::ScannerLog(std::string_view msg) const {}
