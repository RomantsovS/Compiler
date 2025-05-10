#pragma once

#include <memory>
#include <string>

#include "../i_visitor.h"
#include "ast.h"
#include "expr.h"

namespace AST {

struct Assign : public ASTNode {
    Assign() : ASTNode(NodeType::Assign) {}

    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    std::string var;
    std::shared_ptr<Expr> expr;
};

}  // namespace AST
