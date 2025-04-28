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

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void accept(IASTVisitor* visitor) = 0;

    Location loc;
};

using Statements = std::vector<std::shared_ptr<ASTNode>>;

}  // namespace AST
