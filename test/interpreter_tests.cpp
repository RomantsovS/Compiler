#include <gtest/gtest.h>

#include <memory>
#include <sstream>

#include "ast/function.h"
#include "ast/program.h"
#include "ast/type.h"
#include "driver.h"
#include "interpreter.h"
#include "utils.h"

class InterpreterTests : public ::testing::Test {
   protected:
    IR Init(std::istringstream& iss) {
        Driver driver;
        auto res = driver.Run(iss);
        if (res) throw std::runtime_error("Driver run error");

        return driver.GetIR();
    }

    void Exec(const IR& ir, std::ostringstream& oss) {
        Interpreter interpreter(oss);
        interpreter.Exec(ir);
    }
};

TEST_F(InterpreterTests, SimpleMainOK) {
    std::istringstream iss(R"(int main() {})");

    auto ir = Init(iss);

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(ir, oss));

    const std::string expected(R"()");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, PrintIntLiteral) {
    std::istringstream iss(R"(int main() {
print (1);
})");

    auto ir = Init(iss);

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(ir, oss));

    const std::string expected(R"(1
)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, PrintStringLiteral) {
    std::istringstream iss(R"(int main() {
print ("1");
})");

    auto ir = Init(iss);

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(ir, oss));

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

    auto ir = Init(iss);

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(ir, oss));

    const std::string expected(R"(1
)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, PrintIntVarWithoutInitialization) {
    std::istringstream iss(R"(int main() {
int i;
print (i);
})");

    auto ir = Init(iss);

    std::ostringstream oss;
    ExpectThrow(Exec(ir, oss), "3:8: Variable i uninitialized");
}

TEST_F(InterpreterTests, PrintIntArray) {
    std::istringstream iss(R"(int main() {
int i[10];
i[0] = 1;
print (i[0]);
})");

    auto ir = Init(iss);

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(ir, oss));

    const std::string expected(R"(1
)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, PrintIntArrayWithoutInitialization) {
    std::istringstream iss(R"(int main() {
int i[10];
print (i[0]);
})");

    auto ir = Init(iss);

    std::ostringstream oss;
    ExpectThrow(Exec(ir, oss), "array access out of bounds");
}

TEST_F(InterpreterTests, AccessIntArrayOutOfAssignedBoundsFail) {
    std::istringstream iss(R"(int main() {
int i[10];
i[0] = 0;
print (i[1]);
})");

    auto ir = Init(iss);

    std::ostringstream oss;
    ExpectThrow(Exec(ir, oss), "array access out of bounds");
}

TEST_F(InterpreterTests, AccessIntArrayOutOfBoundsFail) {
    std::istringstream iss(R"(int main() {
int i[10];
i[0] = 0;
print (i[10]);
})");

    auto ir = Init(iss);

    std::ostringstream oss;
    ExpectThrow(Exec(ir, oss), "array access out of bounds");
}

// TEST_F(InterpreterTests, FuncCallWithOneArgument) {
//     std::istringstream iss(R"(void foo(int i) {
// print (i);
// }
// int main() {
// foo(1);
// })");

//     auto ir = Init(iss);

//     std::ostringstream oss;
//     EXPECT_NO_THROW(Exec(ir, oss));

//     const std::string expected(R"(1
// )");
//     EXPECT_EQ(oss.str(), expected);
// }
