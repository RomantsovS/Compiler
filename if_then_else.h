#pragma once

#include <string>
#include <vector>

#include "ast.h"
#include "logic_op.h"
#include "name_type.h"
#include "return.h"

struct IfThenElse : public ASTNode {
    LogicOp op;
    Return return_true;
    Return return_false;
};
