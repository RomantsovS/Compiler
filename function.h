#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ast.h"
#include "block.h"
#include "name_type.h"
#include "type.h"

struct Function : public ASTNode {
    std::string name;
    std::vector<NameType> args;
    Block body;
    Type return_type;
};
