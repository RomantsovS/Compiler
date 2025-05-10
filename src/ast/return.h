#pragma once

#include <memory>

#include "../i_visitor.h"
#include "ast.h"
#include "expr.h"

namespace AST {

struct Return : public ASTNode {
    Return() : ASTNode(NodeType::Return), type(Type::Void()) {}
    explicit Return(std::shared_ptr<Expr> ex)
        : ASTNode(NodeType::Return), type(Type::Void()), expr(ex) {}

    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    Type type;
    std::shared_ptr<Expr> expr;
};

}  // namespace AST
