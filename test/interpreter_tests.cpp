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

        driver.SetScannerDebugLevel(1);
        driver.SetParserDebugLevel(1);

        driver.switchInputStream(&iss);

        auto res = driver.parse();
        if (res) return nullptr;

        return driver.GetAST();
    }

    void Exec(std::shared_ptr<AST::ASTNode> ast, std::ostringstream& oss) {
        InterpreterVisitor interpreter_visitor(oss);
        ast->accept(&interpreter_visitor);
    }
};

TEST_F(InterpreterTests, SimpleMainOK) {
    std::istringstream iss(R"(int main() {})");

    auto ast = Init(iss);
    ASSERT_TRUE(ast);

    std::ostringstream oss;
    Exec(ast, oss);

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
    Exec(ast, oss);

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
    Exec(ast, oss);

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
    Exec(ast, oss);

    const std::string expected(R"(1
)");
    EXPECT_EQ(oss.str(), expected);
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
    Exec(ast, oss);

    const std::string expected(R"(1
)");
    EXPECT_EQ(oss.str(), expected);
}
