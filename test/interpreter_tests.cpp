#include <gtest/gtest.h>

#include <memory>
#include <sstream>

#include "ast/function.h"
#include "ast/program.h"
#include "ast/type.h"
#include "driver.h"
#include "interpreter_visitor.h"
#include "utils.h"

class InterpreterTests : public ::testing::Test {
   protected:
    std::shared_ptr<AST::ASTNode> Init(std::istringstream& iss) {
        EzAquarii::Driver driver;

        // driver.SetScannerDebugLevel(1);
        // driver.SetParserDebugLevel(1);

        driver.switchInputStream(&iss);

        auto res = driver.parse();
        if (res) return nullptr;

        return driver.GetAST();
    }

    void Exec(std::shared_ptr<AST::ASTNode> ast, std::ostringstream& oss) {
        Interpreter interpreter(oss);
        interpreter.Exec(ast);
    }
};

TEST_F(InterpreterTests, SimpleMainOK) {
    std::istringstream iss(R"(int main() {})");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(ast, oss));

    const std::string expected(R"()");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, PrintIntLiteral) {
    std::istringstream iss(R"(int main() {
print (1);
})");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(ast, oss));

    const std::string expected(R"(1
)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, PrintStringLiteral) {
    std::istringstream iss(R"(int main() {
print ("1");
})");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(ast, oss));

    const std::string expected(R"(1
)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, PrintIntVar) {
    std::istringstream iss(R"(int main() {
int i;
i = 1;
print (i);
})");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(ast, oss));

    const std::string expected(R"(1
)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, PrintIntVarWithoutInitialization) {
    std::istringstream iss(R"(int main() {
int i;
print (i);
})");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    std::ostringstream oss;
    ExpectThrow(Exec(ast, oss), "3:8: Variable i uninitialized");
}

TEST_F(InterpreterTests, PrintIntArray) {
    std::istringstream iss(R"(int main() {
int i[10];
i[0] = 1;
print (i[0]);
})");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(ast, oss));

    const std::string expected(R"(1
)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, PrintIntArrayWithoutInitialization) {
    std::istringstream iss(R"(int main() {
int i[10];
print (i[0]);
})");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    std::ostringstream oss;
    ExpectThrow(Exec(ast, oss), "array access out of bounds");
}

TEST_F(InterpreterTests, AccessIntArrayOutOfAssignedBoundsFail) {
    std::istringstream iss(R"(int main() {
int i[10];
i[0] = 0;
print (i[1]);
})");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    std::ostringstream oss;
    ExpectThrow(Exec(ast, oss), "array access out of bounds");
}

TEST_F(InterpreterTests, AccessIntArrayOutOfBoundsFail) {
    std::istringstream iss(R"(int main() {
int i[10];
i[0] = 0;
print (i[10]);
})");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    std::ostringstream oss;
    ExpectThrow(Exec(ast, oss), "array access out of bounds");
}

TEST_F(InterpreterTests, FuncCallWithOneArgument) {
    std::istringstream iss(R"(void foo(int i) {
print (i);
}
int main() {
foo(1);
})");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(ast, oss));

    const std::string expected(R"(1
)");
    EXPECT_EQ(oss.str(), expected);
}
