#pragma once

#include <string>

#include "../i_visitor.h"
#include "expr.h"

namespace AST {

struct ArithOp : Expr {
    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    std::string op;
    std::shared_ptr<Expr> lhs;
    std::shared_ptr<Expr> rhs;
};

}  // namespace AST
