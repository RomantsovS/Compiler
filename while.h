#pragma once

#include <memory>
#include <vector>

#include "ast.h"

struct While : public ASTNode {
    std::shared_ptr<ASTNode> condition;
    std::vector<std::shared_ptr<ASTNode>> body;
};
