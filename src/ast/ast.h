#pragma once

#include <iostream>
#include <memory>
#include <vector>

#include "../i_visitor.h"
#include "location.hh"

namespace AST {

struct Location {
    int line;
    int column;
};

inline std::ostream& operator<<(std::ostream& os, const Location& loc) {
    return os << loc.line << ":" << loc.column;
}

enum class NodeType {
    Undefined,
    ArithOp,
    ArrayAccess,
    ArrayDeclaration,
    ArrayAssignment,
    Assign,
    ASTNode,
    BoolLiteral,
    FunCall,
    Function,
    IfThenElse,
    Integer,
    LogicOp,
    Print,
    Program,
    Rand,
    Return,
    StringLiteral,
    Var,
    VarDef,
    While
};

struct ASTNode : public std::enable_shared_from_this<ASTNode> {
    ASTNode(NodeType type) : node_type(type) {}

    virtual ~ASTNode() = default;
    virtual void accept(IASTVisitor* visitor) = 0;

    NodeType node_type = NodeType::Undefined;
    Location loc;
};

using Statements = std::vector<std::shared_ptr<ASTNode>>;

}  // namespace AST
