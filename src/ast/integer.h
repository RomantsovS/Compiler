#pragma once

#include "../i_visitor.h"
#include "expr.h"

namespace AST {

struct Integer : public Expr {
    Integer(int val) : value(val) { type = Type::Int(); }

    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    int value;
};

}  // namespace AST
