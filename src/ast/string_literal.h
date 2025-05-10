#pragma once

#include <string>

#include "../i_visitor.h"
#include "expr.h"

namespace AST {

struct StringLiteral : public Expr {
    explicit StringLiteral(const std::string& val = "")
        : Expr(NodeType::StringLiteral), value(val) {}

    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    std::string value;
};

}  // namespace AST
