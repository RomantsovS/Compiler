#pragma once

#include <memory>
#include <string>
#include <vector>

#include "name_type.h"
#include "statement.h"
#include "type.h"

struct Function : public Statement {
    std::string name;
    std::vector<NameType> args;
    std::vector<NameType> vars;
    std::vector<std::unique_ptr<Function>> fun;
    std::vector<std::unique_ptr<Statement>> body;
    Type return_type;
};
