#pragma once

#include <string>

#include "../i_visitor.h"
#include "ast.h"
#include "type.h"

namespace AST {

struct Expr : public ASTNode {
    explicit Expr(NodeType node_type) : ASTNode(node_type) {}
    Expr(NodeType node_type, Type expr_type)
        : ASTNode(node_type), type(expr_type) {}

    Type type;
};

}  // namespace AST
