#pragma once

#include <memory>
#include <vector>

#include "../i_visitor.h"
#include "ast.h"

namespace AST {

struct While : public ASTNode {
    While() : ASTNode(NodeType::While) {}

    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    std::shared_ptr<ASTNode> condition;
    std::vector<std::shared_ptr<ASTNode>> body;
};

}  // namespace AST
