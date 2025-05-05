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
    void Exec(std::istringstream& iss, std::ostringstream& oss) {
        Driver driver;
        auto res = driver.Run(iss);
        if (res) throw std::runtime_error("Driver run error");

        Interpreter interpreter(driver.GetIR(), oss);
        interpreter.Exec();
    }
};

TEST_F(InterpreterTests, SimpleVoidMainOK) {
    std::istringstream iss(R"(void main() {})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"()");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, SimpleIntMainOK) {
    std::istringstream iss(R"(int main() {return 0;})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"()");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, PrintIntLiteralOK) {
    std::istringstream iss(R"(void main() {
print (1);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(1
)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, PrintStringLiteralOK) {
    std::istringstream iss(R"(void main() {
print ("1");
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(1
)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, PrintIntVarOK) {
    std::istringstream iss(R"(void main() {
int i;
i = 1;
print (i);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(1
)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, PrintIntVarWithoutInitializationFail) {
    std::istringstream iss(R"(void main() {
int i;
print (i);
})");

    std::ostringstream oss;
    ExpectThrow(Exec(iss, oss), "3:8: Variable i uninitialized");
}

TEST_F(InterpreterTests, PrintIntArrayOK) {
    std::istringstream iss(R"(void main() {
int i[10];
i[0] = 1;
print (i[0]);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(1
)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, PrintIntArrayWithoutInitializationFail) {
    std::istringstream iss(R"(void main() {
int i[10];
print (i[0]);
})");

    std::ostringstream oss;
    ExpectThrow(Exec(iss, oss), "array access out of bounds");
}

TEST_F(InterpreterTests, AccessIntArrayOutOfAssignedBoundsFail) {
    std::istringstream iss(R"(void main() {
int i[10];
i[0] = 0;
print (i[1]);
})");

    std::ostringstream oss;
    ExpectThrow(Exec(iss, oss), "array access out of bounds");
}

TEST_F(InterpreterTests, FuncCallWithOneArgumentOK) {
    std::istringstream iss(R"(void foo(int i) {
print (i);
}
void main() {
foo(1);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(1
)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, PrintIntLiteralPlusIntLiteralOK) {
    std::istringstream iss(R"(void main() {
print(1 + 2);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(3
)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, PrintIntVarPlusIntLiteralOK) {
    std::istringstream iss(R"(void main() {
int i;
i = 1;
print(i + 2);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(3
)");
    EXPECT_EQ(oss.str(), expected);
}
