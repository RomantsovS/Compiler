#include <gtest/gtest.h>

#include <memory>
#include <sstream>

#include "ast/function.h"
#include "ast/program.h"
#include "ast/type.h"
#include "interpreter.h"
#include "semantic_visitor.h"

TEST(ASTTests, simplemain) {
    std::istringstream iss(R"(int main() {})");

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

TEST(ASTTests, AssignCheckType) {
    std::istringstream iss(R"(int main() {
        int i;
        i = true;
}
)");

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
    EXPECT_EQ(main_func->body.size(), 2);

    SemanticVisitor semantic_visitor;
    EXPECT_THROW(
        {
            try {
                ast->accept(&semantic_visitor);
            } catch (const std::exception& e) {
                EXPECT_STREQ(e.what(),
                             "3:11: Type mismatch: cannot assign bool to i");
                throw;
            }
        },
        std::runtime_error);
}

TEST(ASTTests, ArrayAssignCheckType) {
    std::istringstream iss(R"(int main() {
        int i[2];
        i[0] = true;
}
)");

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
    EXPECT_EQ(main_func->body.size(), 2);

    SemanticVisitor semantic_visitor;
    EXPECT_THROW(
        {
            try {
                ast->accept(&semantic_visitor);
            } catch (const std::exception& e) {
                EXPECT_STREQ(e.what(),
                             "3:14: Type mismatch: cannot assign bool to i");
                throw;
            }
        },
        std::runtime_error);
}
