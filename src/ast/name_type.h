#pragma once

#include <string>

#include "type.h"

namespace AST {

struct NameType {
    std::string name;
    Type type;
};

}  // namespace AST
