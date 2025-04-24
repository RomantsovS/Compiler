#pragma once

#include <string>

#include "../i_visitor.h"
#include "ast.h"

namespace AST {

struct ArithOp : ASTNode {
    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    std::string op;
    std::shared_ptr<ASTNode> lhs;
    std::shared_ptr<ASTNode> rhs;
};

}  // namespace AST
