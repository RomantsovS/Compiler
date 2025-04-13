#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ast.h"
#include "name_type.h"
#include "type.h"

struct Function : public ASTNode {
    std::string name;
    std::vector<NameType> args;
    std::vector<NameType> vars;
    std::vector<std::unique_ptr<Function>> fun;
    std::vector<std::unique_ptr<ASTNode>> body;
    Type return_type;
};
