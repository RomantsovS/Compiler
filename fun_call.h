#pragma once

#include <memory>
#include <vector>

#include "ast.h"
#include "function.h"

struct FunCall : public ASTNode {
    Function* func;
    std::vector<std::unique_ptr<ASTNode>> args;
};
