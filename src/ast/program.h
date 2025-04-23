#pragma once

#include <memory>
#include <vector>

#include "ast.h"

struct Program : public ASTNode {
    std::vector<std::shared_ptr<ASTNode>> globals;
    std::vector<std::shared_ptr<ASTNode>> functions;
};
