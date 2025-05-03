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

TEST_F(SemanticAnalysisTests, VarUndeclaredVariableCheckOK) {
    std::istringstream iss(R"(int main() {
            int i;
            int j;
            i = j;
        }
        )");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    EXPECT_NO_THROW(ast->accept(&semantic_visitor));
}

TEST_F(SemanticAnalysisTests, VarUndeclaredVariableCheckFail) {
    std::istringstream iss(R"(int main() {
            int i;
            i = j;
        }
        )");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    ExpectThrow(ast->accept(&semantic_visitor), "3:17: Undeclared variable j");
}

TEST_F(SemanticAnalysisTests, AssignUndeclaredVariableCheckOK) {
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

TEST_F(SemanticAnalysisTests, AssignUndeclaredVariableCheckFail) {
    std::istringstream iss(R"(int main() {
        i = 1;
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    ExpectThrow(ast->accept(&semantic_visitor), "2:11: Undeclared variable i");
}

TEST_F(SemanticAnalysisTests, FunCallUndeclaredFuncCheckOK) {
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

TEST_F(SemanticAnalysisTests, FunCallUndeclaredFuncCheckFail) {
    std::istringstream iss(R"(int main() {
        foo();
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    ExpectThrow(ast->accept(&semantic_visitor), "2:9: Undeclared func foo");
}

TEST_F(SemanticAnalysisTests, FunCallWrongNumberOfArgumentsCheckOK) {
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

TEST_F(SemanticAnalysisTests, FunCallWrongNumberOfArgumentsCheckFail) {
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

TEST_F(SemanticAnalysisTests, FunCallWrongArgumentTypeCheckOK) {
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

TEST_F(SemanticAnalysisTests, FunCallWrongArgumentTypeCheckFail) {
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

TEST_F(SemanticAnalysisTests, AssignFromValueCheckTypeOK) {
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

TEST_F(SemanticAnalysisTests, AssignFromValueCheckTypeFail) {
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

TEST_F(SemanticAnalysisTests, AssignFromArrayCheckTypeFail) {
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

TEST_F(SemanticAnalysisTests, AssignFromFunCallReturnCheckTypeOK) {
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

TEST_F(SemanticAnalysisTests, AssignFromFunCallReturnCheckTypeFail) {
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

TEST_F(SemanticAnalysisTests, ArrayAssignFromValueCheckTypeOK) {
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

TEST_F(SemanticAnalysisTests, ArrayAssignFromValueCheckTypeFail) {
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

TEST_F(SemanticAnalysisTests, ArrayAssignFromArrayCheckTypeFail) {
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

TEST_F(SemanticAnalysisTests, ArrayAssignFromFunCallReturnCheckTypeOK) {
    std::istringstream iss(R"(
        int abs() { return 1; }
        int main() {
        int i[10];
        i[0] = abs();
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    EXPECT_NO_THROW(ast->accept(&semantic_visitor));
}

TEST_F(SemanticAnalysisTests, ArrayAssignFromFunCallReturnCheckTypeFail) {
    std::istringstream iss(R"(
        bool abs() { return true; }
        int main() {
        int i[10];
        i = abs();
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    ExpectThrow(ast->accept(&semantic_visitor),
                "5:11: Type mismatch: cannot assign bool to i");
}

TEST_F(SemanticAnalysisTests, FunctionRedeclarationFail) {
    std::istringstream iss(R"(
        bool abs() { return true; }
        int abs() { return 1; }
        int main() {
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    ExpectThrow(ast->accept(&semantic_visitor),
                "3:9: Redeclaration of abs. Previously declared at 2:9");
}

TEST_F(SemanticAnalysisTests, FunctionParamRedeclarationFail) {
    std::istringstream iss(R"(
        bool abs(int i, bool i) { return true; }
        int main() {
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    ExpectThrow(ast->accept(&semantic_visitor),
                "2:9: Redeclaration of i. Previously declared at 0:0");
}
/*
TEST_F(SemanticAnalysisTests, LogicOpTrueLiteralCheckTypeOK) {
    std::istringstream iss(R"(int main() {
        if(true) {}
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    EXPECT_NO_THROW(ast->accept(&semantic_visitor));
}

TEST_F(SemanticAnalysisTests, LogicOpFalseLiteralCheckTypeOK) {
    std::istringstream iss(R"(int main() {
        if(true) {}
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    EXPECT_NO_THROW(ast->accept(&semantic_visitor));
}

TEST_F(SemanticAnalysisTests, LogicOpVarCheckTypeOK) {
    std::istringstream iss(R"(int main() {
        bool b;
        if(b) {}
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    EXPECT_NO_THROW(ast->accept(&semantic_visitor));
}

TEST_F(SemanticAnalysisTests, LogicOpVarCheckTypeFail) {
    std::istringstream iss(R"(int main() {
        int i;
        if(i) {}
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    ExpectThrow(ast->accept(&semantic_visitor),
                "3:11: Type mismatch: cannot assign bool to i");
}

TEST_F(SemanticAnalysisTests, LogicOpCheckTypeFail) {
    std::istringstream iss(R"(int main() {
        if(1) {}
}
)");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    SemanticVisitor semantic_visitor;
    ExpectThrow(ast->accept(&semantic_visitor),
                "3:11: Type mismatch: cannot assign bool to i");
}
*/
