#pragma once

#include <memory>
#include <string>

#include "../i_visitor.h"
#include "ast.h"

namespace AST {

struct Print : public ASTNode {
    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    std::shared_ptr<ASTNode> expr;
};

}  // namespace AST
