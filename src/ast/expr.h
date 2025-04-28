#pragma once

#include <string>

#include "../i_visitor.h"
#include "ast.h"
#include "type.h"

namespace AST {

struct Expr : public ASTNode {
    Type type;
};

}  // namespace AST
