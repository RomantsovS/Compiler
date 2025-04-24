#pragma once

#include <string>

#include "../i_visitor.h"
#include "ast.h"

namespace AST {

struct StringLiteral : public ASTNode {
    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    std::string value;
};

}  // namespace AST
