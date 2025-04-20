#pragma once

#include <memory>
#include <vector>

#include "ast.h"

struct Program : public ASTNode {
    std::shared_ptr<std::vector<std::shared_ptr<ASTNode>>> functions;
};
