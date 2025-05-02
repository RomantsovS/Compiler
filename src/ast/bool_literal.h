#pragma once

#include <memory>
#include <string>

#include "../i_visitor.h"
#include "expr.h"

namespace AST {

struct BoolLiteral : public Expr {
    BoolLiteral(int val) : value(val) { type = Type::Bool(); }

    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    bool value;
};

}  // namespace AST
