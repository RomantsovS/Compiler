#pragma once

#include "../i_visitor.h"
#include "expr.h"

namespace AST {

struct Rand : public Expr {
    Rand() { type = Type::Int(); }

    void accept(IASTVisitor* visitor) override { visitor->visit(this); }
};

}  // namespace AST
