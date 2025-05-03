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

TEST_F(SemanticAnalysisTests, UndeclaredVariableCheckOK) {
    std::istringstream iss(R"(int main() {
        int i;
        i = 1;
    }
    )");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    EXPECT_NO_THROW(ast->accept(&semantic_visitor));
}

TEST_F(SemanticAnalysisTests, UndeclaredVariableCheckFail) {
    std::istringstream iss(R"(int main() {
        i = 1;
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    ExpectThrow(ast->accept(&semantic_visitor), "2:11: Undeclared variable i");
}

TEST_F(SemanticAnalysisTests, UndeclaredFunctionCallCheckOK) {
    std::istringstream iss(R"(
        int foo() {}
        int main() {
        foo();
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    EXPECT_NO_THROW(ast->accept(&semantic_visitor));
}

TEST_F(SemanticAnalysisTests, UndeclaredFunctionCallCheckFail) {
    std::istringstream iss(R"(int main() {
        foo();
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    ExpectThrow(ast->accept(&semantic_visitor), "2:9: Undeclared func foo");
}

TEST_F(SemanticAnalysisTests, FunctionCallWrongNumberOfArgumentsCheckOK) {
    std::istringstream iss(R"(
        int abs(int i, int j) {}
        int main() {
        abs(1, 2);
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    EXPECT_NO_THROW(ast->accept(&semantic_visitor));
}

TEST_F(SemanticAnalysisTests, FunctionCallWrongNumberOfArgumentsCheckFail) {
    std::istringstream iss(R"(
        int abs(int i) {}
        int main() {
        abs(1, 2);
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    ExpectThrow(
        ast->accept(&semantic_visitor),
        "4:9: Incorrect arguments number to call abs. Expected 1 but got 2");
}

TEST_F(SemanticAnalysisTests, FunctionCallWrongArgumentTypeCheckOK) {
    std::istringstream iss(R"(
        int abs(int i) {}
        int main() {
        abs(1);
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    EXPECT_NO_THROW(ast->accept(&semantic_visitor));
}

TEST_F(SemanticAnalysisTests, FunctionCallWrongArgumentTypeCheckFail) {
    std::istringstream iss(R"(
        int abs(int i) {}
        int main() {
        abs(true);
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    ExpectThrow(ast->accept(&semantic_visitor),
                "4:9: Incorrect argument 0 type to call abs. Expected int but "
                "got bool");
}

TEST_F(SemanticAnalysisTests, ValueAssignToValueCheckTypeOK) {
    std::istringstream iss(R"(int main() {
        int i;
        i = 1;
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    EXPECT_NO_THROW(ast->accept(&semantic_visitor));
}

TEST_F(SemanticAnalysisTests, ValueAssignToValueCheckTypeFail) {
    std::istringstream iss(R"(int main() {
        int i;
        i = true;
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    ExpectThrow(ast->accept(&semantic_visitor),
                "3:11: Type mismatch: cannot assign bool to i");
}

TEST_F(SemanticAnalysisTests, ArrayAssignToValueCheckTypeFail) {
    std::istringstream iss(R"(int main() {
        int i;
        int j[2];
        i = j;
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    ExpectThrow(ast->accept(&semantic_visitor),
                "4:11: Type mismatch: cannot assign int[2] to i");
}

TEST_F(SemanticAnalysisTests, ValueAssignToArrayCheckTypeOK) {
    std::istringstream iss(R"(int main() {
        int i[2];
        i[0] = 1;
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    EXPECT_NO_THROW(ast->accept(&semantic_visitor));
}

TEST_F(SemanticAnalysisTests, ValueAssignToArrayCheckTypeFail) {
    std::istringstream iss(R"(int main() {
        int i[2];
        i[0] = true;
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    ExpectThrow(ast->accept(&semantic_visitor),
                "3:14: Type mismatch: cannot assign bool to i");
}

TEST_F(SemanticAnalysisTests, ArrayAssignToArrayCheckTypeFail) {
    std::istringstream iss(R"(int main() {
        int i[2];
        i[0] = i;
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    ExpectThrow(ast->accept(&semantic_visitor),
                "3:14: Type mismatch: cannot assign int[2] to i");
}

TEST_F(SemanticAnalysisTests, FunctionCallReturnAssignToValueCheckTypeOK) {
    std::istringstream iss(R"(
        bool abs() { return true; }
        int main() {
        bool i;
        i = abs();
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    EXPECT_NO_THROW(ast->accept(&semantic_visitor));
}

TEST_F(SemanticAnalysisTests, FunctionCallReturnAssignToValueCheckTypeFail) {
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
