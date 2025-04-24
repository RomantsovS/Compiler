#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../i_visitor.h"
#include "ast.h"
#include "name_type.h"
#include "type.h"

namespace AST {

struct Function : public ASTNode {
    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    std::string name;
    std::vector<NameType> args;
    Statements body;
    Type return_type;
};

}  // namespace AST
