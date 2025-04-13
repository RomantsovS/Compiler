#pragma once

#include <memory>

#include "ast.h"

struct Return : public ASTNode {
    std::unique_ptr<ASTNode> statement;
};
