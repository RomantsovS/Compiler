#pragma once

#include <memory>

struct ASTNode {
    virtual ~ASTNode() = default;
};

    // functions
std::shared_ptr<ASTNode> make_function(const std::string& name);
std::shared_ptr<ASTNode> make_decl(const std::string& name);
