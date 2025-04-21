#pragma once

#include <string>
#include <vector>

#include "name_type.h"

class Function {
   private:
    std::string name_;
    std::vector<NameType> args;
    std::vector<NameType> vars;
    Function* fun;
    std::string body;
};
