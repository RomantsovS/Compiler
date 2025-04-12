#pragma once

#include <string>
#include <vector>

#include "function.h"
#include "integer.h"
#include "statement.h"

struct FunCall : public Statement {
    Function* func;
    std::vector<Integer> args;
};
