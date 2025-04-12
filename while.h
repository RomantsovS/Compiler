#pragma once

#include <memory>
#include <vector>

#include "statement.h"

struct While : public Statement {
    std::unique_ptr<Statement> condition;
    std::vector<std::unique_ptr<Statement>> body;
};
