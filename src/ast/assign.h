#pragma once

#include <memory>
#include <string>

#include "../i_visitor.h"
#include "ast.h"

namespace AST {

struct Assign : public ASTNode {
    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    std::string var;
    std::shared_ptr<ASTNode> st;
};

}  // namespace AST
