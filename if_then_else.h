#pragma once

#include <memory>
#include <string>

#include "ast.h"
#include "logic_op.h"
#include "name_type.h"
#include "return.h"

struct IfThenElse : public ASTNode {
    std::shared_ptr<ASTNode> condition;
    std::shared_ptr<ASTNode> then_branch;
    std::shared_ptr<ASTNode> else_branch;
};
