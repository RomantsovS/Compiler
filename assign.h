#pragma once

#include <string>
#include <memory>

#include "statement.h"

struct Assign : public Statement {
    std::string var;
    std::unique_ptr<Statement> st;
};
