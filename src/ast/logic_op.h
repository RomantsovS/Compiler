#pragma once

#include <string>

#include "../i_visitor.h"
#include "ast.h"
#include "expr.h"
#include "type.h"

namespace AST {

struct LogicOp : public Expr {
    LogicOp() : Expr(NodeType::LogicOp, Type::Bool()) {}

    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    std::string op;
    std::shared_ptr<Expr> lhs;
    std::shared_ptr<Expr> rhs;
};

}  // namespace AST
