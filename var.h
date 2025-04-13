#pragma once

#include <string>

#include "ast.h"
#include "type.h"

struct Var : public ASTNode {
    std::string name;
};

struct VarDef : public Var {
    Type type;
};
