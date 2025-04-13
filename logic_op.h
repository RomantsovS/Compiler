#pragma once

#include <string>

#include "ast.h"
#include "integer.h"
#include "var.h"

struct LogicOp : public ASTNode {
    std::string op;
    std::unique_ptr<ASTNode> lhs;
    std::unique_ptr<ASTNode> rhs;
};
