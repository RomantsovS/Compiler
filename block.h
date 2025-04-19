#pragma once

#include <memory>
#include <vector>

#include "ast.h"

struct Block : public ASTNode {
    std::vector<std::shared_ptr<ASTNode>> body;
};
