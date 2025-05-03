#include <gtest/gtest.h>

#include <memory>
#include <sstream>

#include "ast/function.h"
#include "ast/program.h"
#include "ast/type.h"
#include "interpreter.h"
#include "semantic_visitor.h"

class ParserTests : public ::testing::Test {
   protected:
    std::shared_ptr<AST::ASTNode> Init(std::istringstream& iss) {
        std::shared_ptr<AST::ASTNode> ast;
        EzAquarii::Interpreter interpreter(ast);

        interpreter.SetScannerDebugLevel(1);
        interpreter.SetParserDebugLevel(1);

        interpreter.switchInputStream(&iss);

        auto res = interpreter.parse();
        if (res) ast = nullptr;

        return ast;
    }
};

TEST_F(ParserTests, SimpleMainOK) {
    std::istringstream iss(R"(int main() {})");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

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
