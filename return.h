#pragma once

#include <memory>

#include "ast.h"

struct Return : public ASTNode {
    std::shared_ptr<ASTNode> statement;
};
