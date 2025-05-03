#pragma once

#include <string>
#include <vector>

#include "type.h"

namespace AST {

struct NameType {
    std::string name;
    Type type;
};

using Params = std::vector<NameType>;

}  // namespace AST
