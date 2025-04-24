#pragma once

#include <memory>
#include <vector>

#include "../i_visitor.h"
#include "ast.h"

namespace AST {

struct Program : public ASTNode {
    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    std::vector<std::shared_ptr<ASTNode>> globals;
    std::vector<std::shared_ptr<ASTNode>> functions;
};

}  // namespace AST
