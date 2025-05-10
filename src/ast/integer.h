#pragma once

#include "../i_visitor.h"
#include "expr.h"

namespace AST {

struct Integer : public Expr {
    explicit Integer(int val)
        : Expr(NodeType::Integer, Type::Int()), value(val) {}

    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    int value;
};

}  // namespace AST
