#include <gtest/gtest.h>

#include <memory>
#include <sstream>

#include "ast/function.h"
#include "ast/program.h"
#include "ast/type.h"
#include "interpreter.h"
#include "semantic_visitor.h"

class SemanticAnalysisTests : public ::testing::Test {
   protected:
    std::shared_ptr<AST::ASTNode> Init(std::istringstream& iss) {
        std::shared_ptr<AST::ASTNode> ast;
        EzAquarii::Interpreter interpreter(ast);

        interpreter.switchInputStream(&iss);

        auto res = interpreter.parse();
        if (res) ast = nullptr;

        return ast;
    }
};

#define ExpectThrow(func, msg)                      \
    do {                                            \
        EXPECT_THROW(                               \
            {                                       \
                try {                               \
                    func;                           \
                } catch (const std::exception& e) { \
                    EXPECT_STREQ(e.what(), msg);    \
                    throw;                          \
                }                                   \
            },                                      \
            std::runtime_error);                    \
    } while (0)

TEST_F(SemanticAnalysisTests, UndeclaredVariableCheck) {
    std::istringstream iss(R"(int main() {
        i = 1;
}
)");

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
    EXPECT_EQ(main_func->body.size(), 1);

    SemanticVisitor semantic_visitor;

    ExpectThrow(ast->accept(&semantic_visitor), "2:11: Undeclared variable i");
}

TEST_F(SemanticAnalysisTests, UndeclaredFunctionCallCheck) {
    std::istringstream iss(R"(int main() {
        abs(1);
}
)");

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
    EXPECT_EQ(main_func->body.size(), 1);

    SemanticVisitor semantic_visitor;

    ExpectThrow(ast->accept(&semantic_visitor), "2:9: Undeclared func abs");
}

TEST_F(SemanticAnalysisTests, FunctionCallWrongNumberOfArgumentsCheck) {
    std::istringstream iss(R"(
        int abs(int i) {}
        int main() {
        abs(1, 2);
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    auto prog = std::dynamic_pointer_cast<AST::Program>(ast);
    ASSERT_TRUE(prog);
    ASSERT_EQ(prog->globals.size(), 0);
    ASSERT_EQ(prog->functions.size(), 2);

    auto main_func =
        std::dynamic_pointer_cast<AST::Function>(prog->functions[1]);
    EXPECT_EQ(main_func->name, "main");
    EXPECT_EQ(main_func->return_type, AST::Type::Int());
    EXPECT_EQ(main_func->args.size(), 0);
    EXPECT_EQ(main_func->body.size(), 1);

    SemanticVisitor semantic_visitor;

    ExpectThrow(
        ast->accept(&semantic_visitor),
        "4:9: Incorrect arguments number to call abs. Expected 1 but got 2");
}

TEST_F(SemanticAnalysisTests, FunctionCallWrongArgumentTypeCheck) {
    std::istringstream iss(R"(
        int abs(int i) {}
        int main() {
        abs(true);
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    auto prog = std::dynamic_pointer_cast<AST::Program>(ast);
    ASSERT_TRUE(prog);
    ASSERT_EQ(prog->globals.size(), 0);
    ASSERT_EQ(prog->functions.size(), 2);

    auto main_func =
        std::dynamic_pointer_cast<AST::Function>(prog->functions[1]);
    EXPECT_EQ(main_func->name, "main");
    EXPECT_EQ(main_func->return_type, AST::Type::Int());
    EXPECT_EQ(main_func->args.size(), 0);
    EXPECT_EQ(main_func->body.size(), 1);

    SemanticVisitor semantic_visitor;

    ExpectThrow(ast->accept(&semantic_visitor),
                "4:9: Incorrect argument 0 type to call abs. Expected int but "
                "got bool");
}

TEST_F(SemanticAnalysisTests, ValueAssignToValueCheckType) {
    std::istringstream iss(R"(int main() {
        int i;
        i = true;
}
)");

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
    EXPECT_EQ(main_func->body.size(), 2);

    SemanticVisitor semantic_visitor;

    ExpectThrow(ast->accept(&semantic_visitor),
                "3:11: Type mismatch: cannot assign bool to i");
}

TEST_F(SemanticAnalysisTests, ArrayAssignToValueCheckType) {
    std::istringstream iss(R"(int main() {
        int i;
        int j[2];
        i = j;
}
)");

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
    EXPECT_EQ(main_func->body.size(), 3);

    SemanticVisitor semantic_visitor;

    ExpectThrow(ast->accept(&semantic_visitor),
                "4:11: Type mismatch: cannot assign int[2] to i");
}

TEST_F(SemanticAnalysisTests, ValueAssignToArrayCheckType) {
    std::istringstream iss(R"(int main() {
        int i[2];
        i[0] = true;
}
)");

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
    EXPECT_EQ(main_func->body.size(), 2);

    SemanticVisitor semantic_visitor;
    ExpectThrow(ast->accept(&semantic_visitor),
                "3:14: Type mismatch: cannot assign bool to i");
}

TEST_F(SemanticAnalysisTests, ArrayAssignToArrayCheckType) {
    std::istringstream iss(R"(int main() {
        int i[2];
        i[0] = i;
}
)");

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
    EXPECT_EQ(main_func->body.size(), 2);

    SemanticVisitor semantic_visitor;
    ExpectThrow(ast->accept(&semantic_visitor),
                "3:14: Type mismatch: cannot assign int[2] to i");
}

TEST_F(SemanticAnalysisTests, ValueAssignFromFunctionCallCheckType) {
    std::istringstream iss(R"(
        bool abs() { return true; }
        int main() {
        int i;
        i = abs();
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    ExpectThrow(ast->accept(&semantic_visitor),
                "5:11: Type mismatch: cannot assign bool to i");
}
