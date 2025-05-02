#include <gtest/gtest.h>

#include <memory>
#include <sstream>

#include "ast/function.h"
#include "ast/program.h"
#include "ast/type.h"
#include "interpreter.h"

TEST(ASTTests, simplemain) {
    std::istringstream iss("int main() {}");

    std::shared_ptr<AST::ASTNode> ast;
    EzAquarii::Interpreter interpreter(ast);
    interpreter.switchInputStream(&iss);

    auto res = interpreter.parse();
    ASSERT_EQ(res, 0);

    auto prog = std::dynamic_pointer_cast<AST::Program>(ast);
    ASSERT_TRUE(prog);
    ASSERT_EQ(prog->globals.size(), 0);
    ASSERT_EQ(prog->functions.size(), 1);

    auto main_func =
        std::dynamic_pointer_cast<AST::Function>(prog->functions[0]);
    EXPECT_EQ(main_func->name, "main");
    EXPECT_EQ(main_func->return_type, AST::Type::Int());
    EXPECT_EQ(main_func->args.size(), 0);
    EXPECT_EQ(main_func->body.size(), 0);
}
