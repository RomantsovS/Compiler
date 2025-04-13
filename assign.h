#pragma once

#include <memory>
#include <string>

#include "ast.h"

struct Assign : public ASTNode {
    std::string var;
    std::unique_ptr<ASTNode> st;
};
