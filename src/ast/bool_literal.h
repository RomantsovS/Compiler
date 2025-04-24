#pragma once

#include <memory>
#include <string>

#include "../i_visitor.h"
#include "ast.h"

namespace AST {

struct BoolLiteral : public ASTNode {
    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    bool value;
};

}  // namespace AST
