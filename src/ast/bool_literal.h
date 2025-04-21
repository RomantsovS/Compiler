#pragma once

#include <memory>
#include <string>

#include "ast.h"

struct BoolLiteral : public ASTNode {
    bool value;
};
