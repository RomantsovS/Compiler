#pragma once

#include <string>

#include "../i_visitor.h"
#include "ast.h"
#include "expr.h"
#include "type.h"
namespace AST {

struct ArrayDeclaration : public ASTNode {
    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    std::string name;
    Type type;
};

struct ArrayAccess : public Expr {
    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    std::string name;
    std::shared_ptr<ASTNode> index;
};

struct ArrayAssignment : public ASTNode {
    void accept(IASTVisitor* visitor) override { visitor->visit(this); }

    std::string name;
    std::shared_ptr<ASTNode> index;
    std::shared_ptr<Expr> expr;
};

}  // namespace AST
