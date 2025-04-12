#pragma once

#include <memory>
#include <string>

#include "statement.h"

struct Print : public Statement{
    std::unique_ptr<Statement> st;
};
