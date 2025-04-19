#pragma once

#include <memory>
#include <string>

#include "ast.h"

struct Print : public ASTNode {
    std::shared_ptr<ASTNode> st;
};
