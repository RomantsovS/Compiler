#include <gtest/gtest.h>

#include <memory>
#include <sstream>

#include "ast/function.h"
#include "ast/program.h"
#include "ast/type.h"
#include "parser_driver.h"
#include "semantic_visitor.h"
#include "utils.h"

class SemanticAnalysisTests : public ::testing::Test {
   protected:
    void Exec(std::istringstream& iss) {
        EzAquarii::ParserDriver parser_driver;

        parser_driver.switchInputStream(&iss);

        auto res = parser_driver.parse();
        if (res) throw std::runtime_error("Parser run error");

        auto ast = parser_driver.GetAST();
        ASSERT_TRUE(ast);

        IR ir;
        ir.SetAST(ast);

        SemanticVisitor semantic_visitor(&ir);
        ast->accept(&semantic_visitor);
    }
};

TEST_F(SemanticAnalysisTests, ProgramMainFunctionOK) {
    std::istringstream iss(R"(void main() {
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, ProgramMainFunctionFail) {
    std::istringstream iss(R"(void foo() {
}
)");

    ExpectThrow(Exec(iss), "1:1: Main function isn't found");
}

TEST_F(SemanticAnalysisTests, VarUndeclaredVariableCheckOK) {
    std::istringstream iss(R"(void main() {
int i;
int j;
i = j;
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, VarUndeclaredVariableCheckFail) {
    std::istringstream iss(R"(void main() {
int i;
i = j;
}
)");

    ExpectThrow(Exec(iss), "3:5: Undeclared variable j");
}

TEST_F(SemanticAnalysisTests, AssignUndeclaredVariableCheckOK) {
    std::istringstream iss(R"(void main() {
int i;
i = 1;
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, AssignUndeclaredVariableCheckFail) {
    std::istringstream iss(R"(void main() {
i = 1;
}
)");

    ExpectThrow(Exec(iss), "2:3: Undeclared variable i");
}

TEST_F(SemanticAnalysisTests, FunCallUndeclaredFuncCheckOK) {
    std::istringstream iss(R"(
void foo() {}
void main() {
foo();
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, FunCallUndeclaredFuncCheckFail) {
    std::istringstream iss(R"(void main() {
foo();
}
)");

    ExpectThrow(Exec(iss), "2:1: 'foo' was not declared in this scope");
}

TEST_F(SemanticAnalysisTests, FunCallWrongNumberOfArgumentsCheckOK) {
    std::istringstream iss(R"(
void abs(int i, int j) {}
void main() {
abs(1, 2);
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, FunCallWrongNumberOfArgumentsCheckFail) {
    std::istringstream iss(R"(
void abs(int i) {}
void main() {
abs(1, 2);
}
)");

    ExpectThrow(
        Exec(iss),
        "4:1: Incorrect arguments number to call abs. Expected 1 but got 2");
}

TEST_F(SemanticAnalysisTests, FunCallWrongArgumentTypeCheckOK) {
    std::istringstream iss(R"(
void abs(int i) {}
void main() {
abs(1);
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, FunCallWrongArgumentTypeCheckFail) {
    std::istringstream iss(R"(
void abs(int i) {}
void main() {
abs(true);
}
)");

    ExpectThrow(Exec(iss),
                "4:1: Incorrect argument 0 type to call abs. Expected int but "
                "got bool");
}

TEST_F(SemanticAnalysisTests, FunCallNotAFunctionCallFail) {
    std::istringstream iss(R"(
int foo;
void main() {
foo();
}
)");

    ExpectThrow(Exec(iss), "4:1: 'foo' cannot be used as a function");
}

TEST_F(SemanticAnalysisTests, AssignFromValueCheckTypeOK) {
    std::istringstream iss(R"(void main() {
int i;
i = 1;
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, AssignFromValueCheckTypeFail) {
    std::istringstream iss(R"(void main() {
int i;
i = true;
}
)");

    ExpectThrow(Exec(iss), "3:3: Type mismatch: cannot assign bool to i");
}

TEST_F(SemanticAnalysisTests, AssignFromArrayCheckTypeFail) {
    std::istringstream iss(R"(void main() {
int i;
int j[2];
i = j;
}
)");

    ExpectThrow(Exec(iss), "4:3: Type mismatch: cannot assign int[2] to i");
}

TEST_F(SemanticAnalysisTests, AssignFromFunCallReturnCheckTypeOK) {
    std::istringstream iss(R"(
bool abs() { return true; }
void main() {
bool i;
i = abs();
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, AssignFromFunCallReturnCheckTypeFail) {
    std::istringstream iss(R"(
bool abs() { return true; }
void main() {
int i;
i = abs();
}
)");

    ExpectThrow(Exec(iss), "5:3: Type mismatch: cannot assign bool to i");
}

TEST_F(SemanticAnalysisTests, ArrayAssignmentFromValueCheckTypeOK) {
    std::istringstream iss(R"(void main() {
int i[2];
i[0] = 1;
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, ArrayAssignmentFromValueCheckTypeFail) {
    std::istringstream iss(R"(void main() {
int i[2];
i[0] = true;
}
)");

    ExpectThrow(Exec(iss), "3:6: Type mismatch: cannot assign bool to i");
}

TEST_F(SemanticAnalysisTests, ArrayAssignmentFromArrayCheckTypeFail) {
    std::istringstream iss(R"(void main() {
int i[2];
i[0] = i;
}
)");

    ExpectThrow(Exec(iss), "3:6: Type mismatch: cannot assign int[2] to i");
}

TEST_F(SemanticAnalysisTests, ArrayAssignmentFromFunCallReturnCheckTypeOK) {
    std::istringstream iss(R"(
int abs() { return 0; }
void main() {
int i[10];
i[0] = abs();
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, ArrayAssignmentFromFunCallReturnCheckTypeFail) {
    std::istringstream iss(R"(
bool abs() { return true; }
void main() {
int i[10];
i = abs();
}
)");

    ExpectThrow(Exec(iss), "5:3: Type mismatch: cannot assign bool to i");
}

TEST_F(SemanticAnalysisTests, ArrayAssignmentUndeclaredFail) {
    std::istringstream iss(R"(
void main() {
i[10] = 0;
}
)");

    ExpectThrow(Exec(iss), "3:7: Undeclared array variable i");
}

TEST_F(SemanticAnalysisTests, ArrayAssignmenInBoundsOK) {
    std::istringstream iss(R"(
void main() {
int i[3];
i[0] = 0;
i[1] = 1;
i[2] = 2;
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, ArrayAssignmenOutOfBoundsPositiveFail) {
    std::istringstream iss(R"(
void main() {
int i[3];
i[3] = 0;
}
)");

    ExpectThrow(Exec(iss), "4:6: Array assign out of bounds");
}

TEST_F(SemanticAnalysisTests, FunctionRedeclarationFail) {
    std::istringstream iss(R"(
bool abs() { return true; }
int abs() {  }
void main() {
}
)");

    ExpectThrow(
        Exec(iss),
        "3:1: redefinition of 'int abs()'. Previously defined here 2:1");
}

TEST_F(SemanticAnalysisTests, FunctionRedeclarationWithVarFail) {
    std::istringstream iss(R"(void foo() {}
int foo;
void main() {
}
)");

    ExpectThrow(Exec(iss),
                "2:1: 'int foo' redeclared as different kind of entity");
}

TEST_F(SemanticAnalysisTests, FunctionParamRedeclarationFail) {
    std::istringstream iss(R"(
bool abs(int i, bool i) { return true; }
void main() {
}
)");

    ExpectThrow(Exec(iss),
                "2:1: redefinition of 'bool i'. Previously defined here 2:1");
}

TEST_F(SemanticAnalysisTests, FunctionParamUseSameScopeOK) {
    std::istringstream iss(R"(
void foo(int i) { i = 1; }
void main() {
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, FunctionParamUseOtherScopeFail) {
    std::istringstream iss(R"(
void foo(int i) {}
void main() {
i = 1;
}
)");

    ExpectThrow(Exec(iss), "4:3: Undeclared variable i");
}

TEST_F(SemanticAnalysisTests, FunctionParamRedeclarationWithVarSameScopeFail) {
    std::istringstream iss(R"(
void foo(int i) { int i;}
void main() {
}
)");

    ExpectThrow(Exec(iss),
                "2:19: Redeclaration of i. Previously declared at 2:1");
}

TEST_F(SemanticAnalysisTests, FunctionVoidWithoutReturnOK) {
    std::istringstream iss(R"(
void foo() {}
void main() {
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, FunctionNonVoidWithReturnOK) {
    std::istringstream iss(R"(
void main() {
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, FunctionNonVoidWithoutReturnFail) {
    std::istringstream iss(R"(
int main() {
}
)");

    ExpectThrow(Exec(iss),
                "2:1: in function 'int main()' no return statement in function "
                "returning non-void");
}

TEST_F(SemanticAnalysisTests, FunctionCheckReturnTypeOK) {
    std::istringstream iss(R"(
int main() {
return 1;
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, FunctionCheckReturnTypeFail) {
    std::istringstream iss(R"(
int main() {
return true;
}
)");

    ExpectThrow(Exec(iss),
                "3:1: Type mismatch: main return type is int but got bool");
}

TEST_F(SemanticAnalysisTests, IfThenElseConditionTrueLiteralCheckTypeOK) {
    std::istringstream iss(R"(void main() {
if(true) print(1);
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, IfThenElseConditionFalseLiteralCheckTypeOK) {
    std::istringstream iss(R"(void main() {
if(false) print(1);
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, IfThenElseConditionVarCheckTypeOK) {
    std::istringstream iss(R"(void main() {
bool b;
if(b) print(1);
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, IfThenElseConditionVarCheckTypeFail) {
    std::istringstream iss(R"(void main() {
int i;
if(i) print(1);
}
)");

    ExpectThrow(Exec(iss), "3:1: Type mismatch: condition is not bool");
}

TEST_F(SemanticAnalysisTests, IfThenElseConditionNumberCheckTypeFail) {
    std::istringstream iss(R"(void main() {
if(1) print(1);
}
)");

    ExpectThrow(Exec(iss), "2:1: Type mismatch: condition is not bool");
}

TEST_F(SemanticAnalysisTests, LogicOpTwoNumbersCheckTypeOK) {
    std::istringstream iss(R"(void main() {
1 < 2;
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, LogicOpVarAndNumberCheckTypeOK) {
    std::istringstream iss(R"(void main() {
int i;
i < 1;
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, LogicOpNumberAndVarCheckTypeOK) {
    std::istringstream iss(R"(void main() {
int i;
1 < i;
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, LogicOpNumberAndBoolCheckTypeFail) {
    std::istringstream iss(R"(void main() {
1 < true;
}
)");

    ExpectThrow(Exec(iss),
                "2:3: Type mismatch: cannot perform < for int and bool");
}

TEST_F(SemanticAnalysisTests, ArithOpTwoNumbersCheckTypeOK) {
    std::istringstream iss(R"(void main() {
1 + 2;
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, ArithOpVarAndNumberCheckTypeOK) {
    std::istringstream iss(R"(void main() {
int i;
i + 1;
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, ArithOpNumberAndVarCheckTypeOK) {
    std::istringstream iss(R"(void main() {
int i;
1 + i;
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, ArithOpNumberAndBoolCheckTypeFail) {
    std::istringstream iss(R"(void main() {
1 + true;
}
)");

    ExpectThrow(Exec(iss),
                "2:3: Type mismatch: cannot perform + for int and bool");
}

TEST_F(SemanticAnalysisTests, ArithOpLogicOpParenthesesPrecedenceFail) {
    std::istringstream iss(R"(void main() {
1 + (1 > 2);
})");

    ExpectThrow(Exec(iss),
                "2:3: Type mismatch: cannot perform + for int and bool");
}

TEST_F(SemanticAnalysisTests, VarDefRedeclarationSameScopeFail) {
    std::istringstream iss(R"(
void main() {
int i;
int i;
}
)");

    ExpectThrow(Exec(iss),
                "4:1: Redeclaration of i. Previously declared at 3:1");
}

TEST_F(SemanticAnalysisTests, VarDefRedeclarationWithFuncSameScopeFail) {
    std::istringstream iss(R"(int foo;
void foo() {}
void main() {
}
)");

    ExpectThrow(Exec(iss),
                "2:1: 'void foo' redeclared as different kind of entity");
}

TEST_F(SemanticAnalysisTests, VarDefRedeclarationOfFuncSameScopeFail) {
    std::istringstream iss(R"(void foo() {}
int foo;
void main() {
}
)");

    ExpectThrow(Exec(iss),
                "2:1: 'int foo' redeclared as different kind of entity");
}

TEST_F(SemanticAnalysisTests, VarDefHideParentScopeVarOK) {
    std::istringstream iss(R"(
int i;
void main() {
int i;
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, VarDefHideParentScopeArrayOK) {
    std::istringstream iss(R"(
int i[10];
void main() {
int i;
print (i);
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, VarDefHideParentScopeFuncFail) {
    std::istringstream iss(R"(void foo() {}
void main() {
int foo;
}
)");

    ExpectThrow(Exec(iss),
                "3:1: 'int foo' redeclared as different kind of entity");
}

TEST_F(SemanticAnalysisTests, VarDefDeclaredInOtherFunctionFail) {
    std::istringstream iss(R"(
void foo() {
int i;
}
void main() {
i = 1;
}
)");

    ExpectThrow(Exec(iss), "6:3: Undeclared variable i");
}

TEST_F(SemanticAnalysisTests, VarDefDeclaredInWhileBlockSameScopeOk) {
    std::istringstream iss(R"(
void main() {
while(true) {
int i;
i = 1;
}
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, VarDefDeclaredInWhileBlockOtherScopeFail) {
    std::istringstream iss(R"(
void main() {
while(true) {
int i;
}
i = 1;
}
)");

    ExpectThrow(Exec(iss), "6:3: Undeclared variable i");
}

TEST_F(SemanticAnalysisTests, ArrayDeclarationRedeclarationSameScopeFail) {
    std::istringstream iss(R"(
void main() {
int i[10];
int i[1];
}
)");

    ExpectThrow(Exec(iss),
                "4:1: Redeclaration of i. Previously declared at 3:1");
}

TEST_F(SemanticAnalysisTests,
       ArrayDeclarationRedeclarationWithFuncSameScopeFail) {
    std::istringstream iss(R"(int i[10];
void i() {}
void main() {
}
)");

    ExpectThrow(Exec(iss),
                "2:1: 'void i' redeclared as different kind of entity");
}

TEST_F(SemanticAnalysisTests, ArrayDeclarationHideParentScopeArrayOK) {
    std::istringstream iss(R"(
int i[10];
void main() {
int i[1];
print (i[0]);
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, ArrayDeclarationHideParentScopeVarOK) {
    std::istringstream iss(R"(
int i;
void main() {
int i[1];
print (i[0]);
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, ArrayDeclarationHideParentScopeFuncOK) {
    std::istringstream iss(R"(void foo() {}
void main() {
int foo[1];
print (foo[0]);
}
)");

    ExpectThrow(Exec(iss),
                "3:1: 'int[1] foo' redeclared as different kind of entity");
}

TEST_F(SemanticAnalysisTests, ArrayAccessUndeclaredVariableCheckOK) {
    std::istringstream iss(R"(void main() {
int i[10];
i[0] = 1;
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, ArrayAccessUndeclaredVariableCheckFail) {
    std::istringstream iss(R"(void main() {
i[0] = 1;
}
)");

    ExpectThrow(Exec(iss), "2:6: Undeclared array variable i");
}

TEST_F(SemanticAnalysisTests, ArrayAccessInBoundsOK) {
    std::istringstream iss(R"(
void main() {
int i[3];
int j;
j = i[0];
j = i[1];
j = i[2];
}
)");

    EXPECT_NO_THROW(Exec(iss));
}

TEST_F(SemanticAnalysisTests, ArrayAccessOutOfBoundsFail) {
    std::istringstream iss(R"(
void main() {
int i[3];
int j;
j = i[3];
}
)");

    ExpectThrow(Exec(iss), "5:6: Array assign out of bounds");
}
