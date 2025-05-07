#include <gtest/gtest.h>

#include <memory>
#include <sstream>

#include "ast/function.h"
#include "ast/print.h"
#include "ast/program.h"
#include "ast/string_literal.h"
#include "ast/type.h"
#include "parser_driver.h"

class ParserTests : public ::testing::Test {
   protected:
    std::shared_ptr<AST::ASTNode> Init(std::istringstream& iss) {
        EzAquarii::ParserDriver parser_driver;

        // driver.SetScannerDebugLevel(1);
        // driver.SetParserDebugLevel(1);

        parser_driver.switchInputStream(&iss);

        auto res = parser_driver.parse();
        if (res) return nullptr;

        return parser_driver.GetAST();
    }
};

TEST_F(ParserTests, SimpleMainOK) {
    std::istringstream iss(R"(int main() {})");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    auto prog = std::dynamic_pointer_cast<AST::Program>(ast);
    ASSERT_TRUE(prog);
    ASSERT_EQ(prog->globals.size(), 1);

    auto main_func = std::dynamic_pointer_cast<AST::Function>(prog->globals[0]);
    EXPECT_EQ(main_func->name, "main");
    EXPECT_EQ(main_func->return_type, AST::Type::Int());
    EXPECT_EQ(main_func->args.size(), 0);
    EXPECT_EQ(main_func->body.size(), 0);
}

TEST_F(ParserTests, FunctionNestedFunctionDefinitionFail) {
    std::istringstream iss(R"(
        int main() {
        int foo() {}
}
)");

    auto ast = Init(iss);
    ASSERT_FALSE(ast);
}

TEST_F(ParserTests, IfThenElseConditionTrueLiteralOK) {
    std::istringstream iss(R"(int main() {
        if(true) print(1);
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);
}

TEST_F(ParserTests, IfThenElseConditionFalseLiteralOK) {
    std::istringstream iss(R"(int main() {
        if(false) print(1);
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);
}

TEST_F(ParserTests, IfThenElseConditionVarOK) {
    std::istringstream iss(R"(int main() {
        bool b;
        if(b) print(1);
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);
}

TEST_F(ParserTests, IfThenElseConditionNumberOK) {
    std::istringstream iss(R"(int main() {
        if(1) print(1);
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);
}

TEST_F(ParserTests, ExcapeSymbolsOK) {
    std::istringstream iss(R"(int main() {
print ("\n\033\n\t\034\n\x13\7");
})");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    auto prog = std::dynamic_pointer_cast<AST::Program>(ast);
    ASSERT_TRUE(prog);
    ASSERT_EQ(prog->globals.size(), 1);

    auto main_func = std::dynamic_pointer_cast<AST::Function>(prog->globals[0]);
    ASSERT_EQ(main_func->name, "main");
    EXPECT_EQ(main_func->return_type, AST::Type::Int());
    EXPECT_EQ(main_func->args.size(), 0);
    ASSERT_EQ(main_func->body.size(), 1);

    auto print_st = std::dynamic_pointer_cast<AST::Print>(main_func->body[0]);
    ASSERT_TRUE(print_st);

    auto str_lit =
        std::dynamic_pointer_cast<AST::StringLiteral>(print_st->expr);
    ASSERT_TRUE(str_lit);

    EXPECT_EQ(str_lit->value, "\n\033\n\t\034\n\x13\7");
}
