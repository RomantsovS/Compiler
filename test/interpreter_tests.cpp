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

    const std::string expected(R"(1)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, PrintStringLiteralOK) {
    std::istringstream iss(R"(void main() {
print ("1");
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(1)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, PrintBoolLiteralTrueOK) {
    std::istringstream iss(R"(void main() {
print (true);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(1)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, PrintBoolLiteralFalseOK) {
    std::istringstream iss(R"(void main() {
print (false);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(0)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, PrintEscapeSymbolsOK) {
    std::istringstream iss(R"(void main() {
print ("\n\033\n\t\034\n\x13\7");
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected("\n\033\n\t\034\n\x13\7");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, AccessIntVarOK) {
    std::istringstream iss(R"(void main() {
int i;
i = 1;
print (i);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(1)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, AccessIntVarWithoutInitializationFail) {
    std::istringstream iss(R"(void main() {
int i;
print (i);
})");

    std::ostringstream oss;
    ExpectThrow(Exec(iss, oss), "3:8: Variable i uninitialized");
}

TEST_F(InterpreterTests, AccessIntArrayOK) {
    std::istringstream iss(R"(void main() {
int i[10];
i[0] = 1;
print (i[0]);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(1)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, AccessIntArrayWithoutInitializationFail) {
    std::istringstream iss(R"(void main() {
int i[10];
print (i[0]);
})");

    std::ostringstream oss;
    ExpectThrow(Exec(iss, oss), "3:9: array access out of bounds");
}

TEST_F(InterpreterTests, AccessIntArrayOutOfAssignedBoundsFail) {
    std::istringstream iss(R"(void main() {
int i[10];
i[0] = 0;
print (i[1]);
})");

    std::ostringstream oss;
    ExpectThrow(Exec(iss, oss), "4:9: array access out of bounds");
}

TEST_F(InterpreterTests, PrintArrayFail) {
    std::istringstream iss(R"(void main() {
int i[10];
print (i);
})");

    std::ostringstream oss;
    ExpectThrow(Exec(iss, oss), "call ArrayObject Print");
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

    const std::string expected(R"(1)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, FuncCallWithMultipleArgumentsOK) {
    std::istringstream iss(R"(void foo(int i, int j, bool b) {
print (i);
print (j);
print (b);
}
void main() {
foo(3, 2, true);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(321)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, IntLiteralPlusIntLiteralOK) {
    std::istringstream iss(R"(void main() {
print(1 + 2);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(3)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, IntLiteralMinusIntLiteralOK) {
    std::istringstream iss(R"(void main() {
print(10 - 2);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(8)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, IntLiteralMultiplyIntLiteralOK) {
    std::istringstream iss(R"(void main() {
print(3 * 2);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(6)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, IntLiteralDivideIntLiteralOK) {
    std::istringstream iss(R"(void main() {
print(10 / 2);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(5)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, IntLiteralModIntLiteralOK) {
    std::istringstream iss(R"(void main() {
print(13 % 2);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(1)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, IntVarPlusIntLiteralOK) {
    std::istringstream iss(R"(void main() {
int i;
i = 1;
print(i + 2);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(3)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, IntLiteralLessIntLiteralTrueOK) {
    std::istringstream iss(R"(void main() {
print(1 < 2);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(1)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, IntLiteralLessIntLiteralFalseOK) {
    std::istringstream iss(R"(void main() {
print(1 > 2);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(0)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, IntLiteralGreaterIntLiteralTrueOK) {
    std::istringstream iss(R"(void main() {
print(10 > 2);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(1)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, IntLiteralGreaterIntLiteralFalseOK) {
    std::istringstream iss(R"(void main() {
print(10 < 2);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(0)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, IntVarLessIntLiteralOK) {
    std::istringstream iss(R"(void main() {
int i;
i = 1;
print(i < 2);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(1)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, ReassignIntVarOK) {
    std::istringstream iss(R"(void main() {
int i;
i = 1;
print(i);
i = 2;
print(i);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(12)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, HideIntVarWithFunctionOK) {
    std::istringstream iss(R"(void foo(int i) {
i = 2;
print (i);
}
void main() {
int i;
i = 1;
print(i);
foo(i);
print(i);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(121)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, HideIntVarWithWhileOK) {
    std::istringstream iss(R"(void main() {
int i;
i = 1;
print(i);
int j;
j = 2;
print(j);
while(i < 3) {
int j;
j = 1;
print(j);
print(i);
i = i + 1;
}
print(i);
print(j);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(12111232)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, ReassignIntArrayOK) {
    std::istringstream iss(R"(void main() {
int i[10];
i[0] = 1;
print(i[0]);
i[0] = 2;
print(i[0]);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(12)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, IfThenTrueOK) {
    std::istringstream iss(R"(void main() {
if(1 < 2) print (1);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(1)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, IfThenFalseOK) {
    std::istringstream iss(R"(void main() {
if(1 > 2) print (1);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"()");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, IfThenElseOK) {
    std::istringstream iss(R"(void main() {
if(1 > 2) print (1);
else print (2);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(2)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, WhileFalseConditionOK) {
    std::istringstream iss(R"(void main() {
while(false) {
print (1);
}
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"()");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, WhileLoopOK) {
    std::istringstream iss(R"(void main() {
int i;
i = 1;
while(i < 5) {
print (i);
i = i + 1;
}
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(1234)");
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(InterpreterTests, RandOK) {
    std::istringstream iss(R"(void main() {
print ((rand() % 10) < 10);
})");

    std::ostringstream oss;
    EXPECT_NO_THROW(Exec(iss, oss));

    const std::string expected(R"(1)");
    EXPECT_EQ(oss.str(), expected);
}
