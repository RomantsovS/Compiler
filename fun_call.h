#pragma once

#include <memory>
#include <vector>

#include "ast.h"
#include "function.h"

struct FunCall : public ASTNode {
    std::shared_ptr<Function> func;
    std::vector<std::shared_ptr<ASTNode>> args;
};
