#pragma once

#include <memory>
#include <string>

#include "../i_visitor.h"
#include "expr.h"

namespace AST {

struct BoolLiteral : public Expr {
    explicit BoolLiteral(int val)
        : Expr(NodeType::BoolLiteral, Type::Bool()), value(val) {}

    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    bool value;
};

}  // namespace AST
