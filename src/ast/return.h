#pragma once

#include <memory>

#include "../i_visitor.h"
#include "ast.h"
#include "expr.h"

namespace AST {

struct Return : public ASTNode {
    Return() : type(Type::Void()) {}
    Return(std::shared_ptr<Expr> ex) : expr(ex) {}

    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    Type type;
    std::shared_ptr<Expr> expr;
};

}  // namespace AST
