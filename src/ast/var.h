#pragma once

#include <string>

#include "../i_visitor.h"
#include "ast.h"
#include "expr.h"
#include "type.h"

namespace AST {

struct Var : public Expr {
    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    std::string name;
};

struct VarDef : public Var {
    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    Type type;
};

}  // namespace AST
