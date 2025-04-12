#pragma once

#include <string>

#include "integer.h"
#include "statement.h"
#include "var.h"

struct LogicOp : public Statement {
    std::string op;
    std::unique_ptr<Statement> lhs;
    std::unique_ptr<Statement> rhs;
};
