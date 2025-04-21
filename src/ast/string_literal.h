#pragma once

#include <memory>
#include <string>

#include "ast.h"

struct StringLiteral : public ASTNode {
    std::string value;
};
