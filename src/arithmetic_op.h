#pragma once

#include <string>

#include "ast.h"
#include "integer.h"
#include "var.h"

struct ArithOp : ASTNode {
    std::string op;
    std::shared_ptr<ASTNode> lhs;
    std::shared_ptr<ASTNode> rhs;
};
