#pragma once

#include <string>
#include <vector>

#include "logic_op.h"
#include "name_type.h"
#include "return.h"
#include "statement.h"

struct IfThenElse : public Statement {
    LogicOp op;
    Return return_true;
    Return return_false;
};
