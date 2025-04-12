#pragma once

#include <string>

#include "statement.h"
#include "type.h"

struct Var : public Statement {
    std::string name;
};

struct VarDef : public Var {
    Type type;
};
