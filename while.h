#pragma once

#include <memory>
#include <vector>

#include "ast.h"

struct While : public ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> body;
};
