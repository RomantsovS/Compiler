#pragma once

#include "iostream"
#include "ir.h"
#include "parser_driver.h"

class Driver {
   public:
    int Run(std::istream& input);

    IR GetIR() const;

   private:
    EzAquarii::ParserDriver parser_driver_;

    IR ir_;
};
