#pragma once

#include <memory>
#include <vector>

#include "function.h"
#include "statement.h"

struct FunCall : public Statement {
    Function* func;
    std::vector<std::unique_ptr<Statement>> args;
};
