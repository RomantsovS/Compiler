#include <gtest/gtest.h>

#include <sstream>

#include "../src/ast/var.h"
#include "../src/interpreter.h"

TEST(ASTTests, VarNodeHoldsCorrectName) {
    std::istringstream iss("int main() {}");

    std::shared_ptr<AST::ASTNode> ast;
    EzAquarii::Interpreter interpreter(ast);
    interpreter.switchInputStream(&iss);

    auto res = interpreter.parse();
    EXPECT_FALSE(res);
}
