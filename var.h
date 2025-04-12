#pragma once

#include <string>
#include "statement.h"

struct Var : public Statement {
    std::string name;
};
