#pragma once

#include <memory>
#include <string>

#include "../i_visitor.h"
#include "ast.h"
#include "expr.h"

namespace AST {

struct IfThenElse : public ASTNode {
    IfThenElse() : ASTNode(NodeType::IfThenElse) {}

    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    std::shared_ptr<Expr> condition;
    std::vector<std::shared_ptr<ASTNode>> then_branch;
    std::vector<std::shared_ptr<ASTNode>> else_branch;
};

}  // namespace AST
