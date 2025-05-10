#pragma once

#include <string>

#include "../i_visitor.h"
#include "ast.h"
#include "expr.h"
#include "type.h"

namespace AST {

struct Var : public Expr {
    Var() : Expr(NodeType::Var) {}

    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    std::string name;
};

struct VarDef : public Expr {
    VarDef() : Expr(NodeType::VarDef) {}

    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    std::string name;
    Type type;
};

}  // namespace AST
