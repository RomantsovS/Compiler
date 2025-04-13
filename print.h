#pragma once

#include <memory>
#include <string>

#include "ast.h"

struct Print : public ASTNode {
    std::unique_ptr<ASTNode> st;
};
