#pragma once

#include "../i_visitor.h"
#include "expr.h"

namespace AST {

struct Integer : public Expr {
    Integer(int value) : val(value) { type = Type::Int(); }

    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    int val;
};

}  // namespace AST
