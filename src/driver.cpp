#include "driver.h"

#include <iostream>

#include "semantic_visitor.h"

int Driver::Run(std::istream& input) {
    parser_driver_.switchInputStream(&input);

    int res = parser_driver_.parse();
    if (res) return res;

    ir_.SetAST(parser_driver_.GetAST());

    SemanticVisitor semantic_visitor(&ir_);
    try {
        ir_.GetAST()->accept(&semantic_visitor);
    } catch (const std::exception& ex) {
        std::cout << "\033[31mError: " << ex.what() << "\033[0m\n";
    }

    return res;
};

IR Driver::GetIR() const { return ir_; }