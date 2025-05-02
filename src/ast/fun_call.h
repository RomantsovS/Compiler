#pragma once

#include <string>
#include <vector>

#include "../i_visitor.h"
#include "expr.h"

namespace AST {

struct FunCall : public Expr {
    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    std::string name;
    std::vector<std::shared_ptr<AST::Expr>> args;
};

}  // namespace AST
