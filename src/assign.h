#pragma once

#include <memory>
#include <string>

#include "ast.h"

struct Assign : public ASTNode {
    std::string var;
    std::shared_ptr<ASTNode> st;
};
