#include <cassert>
#include <deque>
#include <iostream>
#include <stack>
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
#include "interpreter.h"
#include "parser.hpp"
#include "pretty_print_visitor.h"
#include "scanner.h"

int main() {
    std::shared_ptr<AST::ASTNode> ast;
    EzAquarii::Interpreter interpreter(ast);

    auto res = interpreter.parse();
    if (res) return res;

    PrettyPrintVisitor print_visitor(std::cout);
    ast->accept(&print_visitor);

    return 0;
}
