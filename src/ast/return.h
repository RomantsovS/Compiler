#pragma once

#include <memory>

#include "../i_visitor.h"
#include "ast.h"

namespace AST {

struct Return : public ASTNode {
    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    std::shared_ptr<ASTNode> statement;
};

}  // namespace AST
