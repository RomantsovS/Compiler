#pragma once

#include <string>

#include "integer.h"
#include "statement.h"
#include "var.h"

struct ArithOp : Statement {
    std::string op;
    Var var;
    Integer value;
};
