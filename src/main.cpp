#include <cassert>
#include <deque>
#include <iostream>
#include <stdexcept>
#include <typeinfo>
#include <unordered_set>

#include "ast/arithmetic_op.h"
#include "ast/array.h"
#include "ast/assign.h"
#include "ast/ast.h"
#include "ast/bool_literal.h"
#include "ast/fun_call.h"
#include "ast/function.h"
#include "ast/if_then_else.h"
#include "ast/integer.h"
#include "ast/logic_op.h"
#include "ast/print.h"
#include "ast/program.h"
#include "ast/return.h"
#include "ast/string_literal.h"
#include "ast/type.h"
#include "ast/var.h"
#include "ast/while.h"
#include "driver.h"
#include "interpreter.h"
#include "parser.hpp"
#include "pretty_print_visitor.h"
#include "print_visitor.h"
#include "scanner.h"
#include "semantic_visitor.h"

int main() {
    Driver driver;

    auto res = driver.Run(std::cin);
    if (res) return res;

    auto ir = driver.GetIR();

    std::cout << "\n\n";

    PrintVisitor print_visitor(std::cout);
    ir.GetAST()->accept(&print_visitor);

    std::cout << "\n\n";

    PrettyPrintVisitor pretty_print_visitor(std::cout);
    ir.GetAST()->accept(&pretty_print_visitor);

    std::cout << "\n\n";

    Interpreter interpreter(std::cout);

    try {
        interpreter.Exec(ir);
    } catch (const std::exception& ex) {
        std::cout << "\033[31mError: " << ex.what() << "\033[0m\n";
    }

    return 0;
}
