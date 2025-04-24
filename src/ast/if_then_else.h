#pragma once

#include <memory>
#include <string>

#include "../i_visitor.h"
#include "ast.h"

namespace AST {

struct IfThenElse : public ASTNode {
    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    std::shared_ptr<ASTNode> condition;
    std::shared_ptr<ASTNode> then_branch;
    std::shared_ptr<ASTNode> else_branch;
};

}  // namespace AST
