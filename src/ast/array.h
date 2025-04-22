#pragma once

#include <string>

#include "ast.h"
#include "type.h"

struct ArrayDeclaration : public ASTNode {
    std::string name;
    Type type;
};

struct ArrayAccess : public ASTNode {
    std::string name;
    std::shared_ptr<ASTNode> index;
};

struct ArrayAssignment : public ASTNode {
    std::string name;
    std::shared_ptr<ASTNode> index;
    std::shared_ptr<ASTNode> value;
};
