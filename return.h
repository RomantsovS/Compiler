#pragma once

#include <memory>

#include "statement.h"

struct Return : public Statement {
    std::unique_ptr<Statement> statement;
};
