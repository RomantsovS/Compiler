#pragma once

#include "../i_visitor.h"
#include "ast.h"

namespace AST {

struct Integer : public ASTNode {
    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    int val;
};

}  // namespace AST
