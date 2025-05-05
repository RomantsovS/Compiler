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
    std::shared_ptr<AST::ASTNode> Init(std::istringstream& iss) {
        EzAquarii::ParserDriver parser_driver;

        parser_driver.switchInputStream(&iss);

        auto res = parser_driver.parse();
        if (res) return nullptr;

        return parser_driver.GetAST();
    }

    void CallNoThrow(std::istringstream& iss) {
        auto ast = Init(iss);
        ASSERT_TRUE(ast);

        IR ir;
        ir.SetAST(ast);

        SemanticVisitor semantic_visitor(&ir);
        EXPECT_NO_THROW(ast->accept(&semantic_visitor));
    }

    void CallThrow(std::istringstream& iss, const std::string& msg) {
        auto ast = Init(iss);
        ASSERT_TRUE(ast);

        IR ir;
        ir.SetAST(ast);

        SemanticVisitor semantic_visitor(&ir);
        ExpectThrow(ast->accept(&semantic_visitor), msg.c_str());
    }
};

TEST_F(SemanticAnalysisTests, ProgramMainFunctionOK) {
    std::istringstream iss(R"(int main() {
        return 0;
    }
    )");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, ProgramMainFunctionFail) {
    std::istringstream iss(R"(int foo() {
            return 0;
        }
        )");

    CallThrow(iss, "1:1: Main function isn't found");
}

TEST_F(SemanticAnalysisTests, VarUndeclaredVariableCheckOK) {
    std::istringstream iss(R"(int main() {
            int i;
            int j;
            i = j;
            return 0;
        }
        )");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, VarUndeclaredVariableCheckFail) {
    std::istringstream iss(R"(int main() {
            int i;
            i = j;
            return 0;
        }
        )");

    CallThrow(iss, "3:17: Undeclared variable j");
}

TEST_F(SemanticAnalysisTests, AssignUndeclaredVariableCheckOK) {
    std::istringstream iss(R"(int main() {
        int i;
        i = 1;
        return 0;
    }
    )");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, AssignUndeclaredVariableCheckFail) {
    std::istringstream iss(R"(int main() {
        i = 1;
        return 0;
}
)");

    CallThrow(iss, "2:11: Undeclared variable i");
}

TEST_F(SemanticAnalysisTests, FunCallUndeclaredFuncCheckOK) {
    std::istringstream iss(R"(
        int foo() {return 0;}
        int main() {
        foo();
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, FunCallUndeclaredFuncCheckFail) {
    std::istringstream iss(R"(int main() {
        foo();
        return 0;
}
)");

    CallThrow(iss, "2:9: Undeclared func foo");
}

TEST_F(SemanticAnalysisTests, FunCallWrongNumberOfArgumentsCheckOK) {
    std::istringstream iss(R"(
        int abs(int i, int j) {return 0;}
        int main() {
        abs(1, 2);
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, FunCallWrongNumberOfArgumentsCheckFail) {
    std::istringstream iss(R"(
        int abs(int i) {return 0;}
        int main() {
        abs(1, 2);
        return 0;
}
)");

    CallThrow(
        iss,
        "4:9: Incorrect arguments number to call abs. Expected 1 but got 2");
}

TEST_F(SemanticAnalysisTests, FunCallWrongArgumentTypeCheckOK) {
    std::istringstream iss(R"(
        int abs(int i) {return 0;}
        int main() {
        abs(1);
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, FunCallWrongArgumentTypeCheckFail) {
    std::istringstream iss(R"(
        int abs(int i) {return 0;}
        int main() {
        abs(true);
        return 0;
}
)");

    CallThrow(iss,
              "4:9: Incorrect argument 0 type to call abs. Expected int but "
              "got bool");
}

TEST_F(SemanticAnalysisTests, AssignFromValueCheckTypeOK) {
    std::istringstream iss(R"(int main() {
        int i;
        i = 1;
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, AssignFromValueCheckTypeFail) {
    std::istringstream iss(R"(int main() {
        int i;
        i = true;
        return 0;
}
)");

    CallThrow(iss, "3:11: Type mismatch: cannot assign bool to i");
}

TEST_F(SemanticAnalysisTests, AssignFromArrayCheckTypeFail) {
    std::istringstream iss(R"(int main() {
        int i;
        int j[2];
        i = j;
        return 0;
}
)");

    CallThrow(iss, "4:11: Type mismatch: cannot assign int[2] to i");
}

TEST_F(SemanticAnalysisTests, AssignFromFunCallReturnCheckTypeOK) {
    std::istringstream iss(R"(
        bool abs() { return true; }
        int main() {
        bool i;
        i = abs();
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, AssignFromFunCallReturnCheckTypeFail) {
    std::istringstream iss(R"(
        bool abs() { return true; }
        int main() {
        int i;
        i = abs();
        return 0;
}
)");

    CallThrow(iss, "5:11: Type mismatch: cannot assign bool to i");
}

TEST_F(SemanticAnalysisTests, ArrayAssignmentFromValueCheckTypeOK) {
    std::istringstream iss(R"(int main() {
        int i[2];
        i[0] = 1;
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, ArrayAssignmentFromValueCheckTypeFail) {
    std::istringstream iss(R"(int main() {
        int i[2];
        i[0] = true;
        return 0;
}
)");

    CallThrow(iss, "3:14: Type mismatch: cannot assign bool to i");
}

TEST_F(SemanticAnalysisTests, ArrayAssignmentFromArrayCheckTypeFail) {
    std::istringstream iss(R"(int main() {
        int i[2];
        i[0] = i;
        return 0;
}
)");

    CallThrow(iss, "3:14: Type mismatch: cannot assign int[2] to i");
}

TEST_F(SemanticAnalysisTests, ArrayAssignmentFromFunCallReturnCheckTypeOK) {
    std::istringstream iss(R"(
        int abs() { return 0; }
        int main() {
        int i[10];
        i[0] = abs();
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, ArrayAssignmentFromFunCallReturnCheckTypeFail) {
    std::istringstream iss(R"(
        bool abs() { return true; }
        int main() {
        int i[10];
        i = abs();
        return 0;
}
)");

    CallThrow(iss, "5:11: Type mismatch: cannot assign bool to i");
}

TEST_F(SemanticAnalysisTests, ArrayAssignmentUndeclaredFail) {
    std::istringstream iss(R"(
        int main() {
        i[10] = 0;
        return 0;
}
)");

    CallThrow(iss, "3:15: Undeclared array variable i");
}

TEST_F(SemanticAnalysisTests, ArrayAssignmenInBoundsOK) {
    std::istringstream iss(R"(
        int main() {
        int i[3];
        i[0] = 0;
        i[1] = 1;
        i[2] = 2;
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, ArrayAssignmenOutOfBoundsPositiveFail) {
    std::istringstream iss(R"(
        int main() {
        int i[3];
        i[3] = 0;
        return 0;
}
)");

    CallThrow(iss, "4:14: Array assign out of bounds");
}

TEST_F(SemanticAnalysisTests, FunctionRedeclarationFail) {
    std::istringstream iss(R"(
        bool abs() { return true; }
        int abs() { return 0; }
        int main() {
        return 0;
}
)");

    CallThrow(iss, "3:9: Redeclaration of abs. Previously declared at 2:9");
}

TEST_F(SemanticAnalysisTests, FunctionParamRedeclarationFail) {
    std::istringstream iss(R"(
        bool abs(int i, bool i) { return true; }
        int main() {
        return 0;
}
)");

    CallThrow(iss, "2:9: Redeclaration of i. Previously declared at 2:9");
}

TEST_F(SemanticAnalysisTests, FunctionParamUseSameScopeOK) {
    std::istringstream iss(R"(
        void foo(int i) { i = 1; }
        int main() {
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, FunctionParamUseOtherScopeFail) {
    std::istringstream iss(R"(
        void foo(int i) {}
        int main() {
        i = 1;
        return 0;
}
)");

    CallThrow(iss, "4:11: Undeclared variable i");
}

TEST_F(SemanticAnalysisTests, FunctionParamRedeclarationWithVarSameScopeFail) {
    std::istringstream iss(R"(
        void foo(int i) { int i;}
        int main() {
        return 0;
}
)");

    CallThrow(iss, "2:27: Redeclaration of i. Previously declared at 2:9");
}

TEST_F(SemanticAnalysisTests, FunctionVoidWithoutReturnOK) {
    std::istringstream iss(R"(
        void foo() {}
        int main() {
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, FunctionNonVoidWithReturnOK) {
    std::istringstream iss(R"(
        int main() {
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, FunctionNonVoidWithoutReturnFail) {
    std::istringstream iss(R"(
        int main() {
}
)");

    CallThrow(iss, "2:9: non-void function main does not return a value");
}

TEST_F(SemanticAnalysisTests, FunctionCheckReturnTypeOK) {
    std::istringstream iss(R"(
        int main() {
        return 1;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, FunctionCheckReturnTypeFail) {
    std::istringstream iss(R"(
        int main() {
        return true;
}
)");

    CallThrow(iss, "3:9: Type mismatch: main return type is int but got bool");
}

TEST_F(SemanticAnalysisTests, IfThenElseConditionTrueLiteralCheckTypeOK) {
    std::istringstream iss(R"(int main() {
        if(true) print(1);
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, IfThenElseConditionFalseLiteralCheckTypeOK) {
    std::istringstream iss(R"(int main() {
        if(false) print(1);
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, IfThenElseConditionVarCheckTypeOK) {
    std::istringstream iss(R"(int main() {
        bool b;
        if(b) print(1);
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, IfThenElseConditionVarCheckTypeFail) {
    std::istringstream iss(R"(int main() {
        int i;
        if(i) print(1);
        return 0;
}
)");

    CallThrow(iss, "3:9: Type mismatch: condition is not bool");
}

TEST_F(SemanticAnalysisTests, IfThenElseConditionNumberCheckTypeFail) {
    std::istringstream iss(R"(int main() {
        if(1) print(1);
        return 0;
}
)");

    CallThrow(iss, "2:9: Type mismatch: condition is not bool");
}

TEST_F(SemanticAnalysisTests, LogicOpTwoNumbersCheckTypeOK) {
    std::istringstream iss(R"(int main() {
        1 < 2;
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, LogicOpVarAndNumberCheckTypeOK) {
    std::istringstream iss(R"(int main() {
        int i;
        i < 1;
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, LogicOpNumberAndVarCheckTypeOK) {
    std::istringstream iss(R"(int main() {
        int i;
        1 < i;
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, LogicOpNumberAndBoolCheckTypeFail) {
    std::istringstream iss(R"(int main() {
        1 < true;
        return 0;
}
)");

    CallThrow(iss, "2:11: Type mismatch: cannot perform < for int and bool");
}

TEST_F(SemanticAnalysisTests, ArithOpTwoNumbersCheckTypeOK) {
    std::istringstream iss(R"(int main() {
        1 + 2;
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, ArithOpVarAndNumberCheckTypeOK) {
    std::istringstream iss(R"(int main() {
        int i;
        i + 1;
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, ArithOpNumberAndVarCheckTypeOK) {
    std::istringstream iss(R"(int main() {
        int i;
        1 + i;
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, ArithOpNumberAndBoolCheckTypeFail) {
    std::istringstream iss(R"(int main() {
        1 + true;
        return 0;
}
)");

    CallThrow(iss, "2:11: Type mismatch: cannot perform + for int and bool");
}

TEST_F(SemanticAnalysisTests, VarDefRedeclarationSameScopeFail) {
    std::istringstream iss(R"(
        int main() {
        int i;
        int i;
        return 0;
}
)");

    CallThrow(iss, "4:9: Redeclaration of i. Previously declared at 3:9");
}

TEST_F(SemanticAnalysisTests, VarDefHideParentScopeOK) {
    std::istringstream iss(R"(
        int i;
        int main() {
        int i;
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, VarDefDeclaredInOtherFunctionFail) {
    std::istringstream iss(R"(
        void foo() {
        int i;
        }
        int main() {
        i = 1;
        return 0;
}
)");

    CallThrow(iss, "6:11: Undeclared variable i");
}

TEST_F(SemanticAnalysisTests, VarDefDeclaredInWhileBlockSameScopeOk) {
    std::istringstream iss(R"(
        int main() {
        while(true) {
        int i;
        i = 1;
        }
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, VarDefDeclaredInWhileBlockOtherScopeFail) {
    std::istringstream iss(R"(
        int main() {
        while(true) {
        int i;
        }
        i = 1;
        return 0;
}
)");

    CallThrow(iss, "6:11: Undeclared variable i");
}

TEST_F(SemanticAnalysisTests, ArrayDeclarationRedeclarationSameScopeFail) {
    std::istringstream iss(R"(
        int main() {
        int i[10];
        int i[1];
        return 0;
}
)");

    CallThrow(iss, "4:9: Redeclaration of i. Previously declared at 3:9");
}

TEST_F(SemanticAnalysisTests, ArrayDeclarationHideParentScopeOK) {
    std::istringstream iss(R"(
        int i[10];
        int main() {
        int i[1];
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, ArrayAccessUndeclaredVariableCheckOK) {
    std::istringstream iss(R"(int main() {
        int i[10];
        i[0] = 1;
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, ArrayAccessUndeclaredVariableCheckFail) {
    std::istringstream iss(R"(int main() {
        i[0] = 1;
        return 0;
}
)");

    CallThrow(iss, "2:14: Undeclared array variable i");
}

TEST_F(SemanticAnalysisTests, ArrayAccessInBoundsOK) {
    std::istringstream iss(R"(
        int main() {
        int i[3];
        int j;
        j = i[0];
        j = i[1];
        j = i[2];
        return 0;
}
)");

    CallNoThrow(iss);
}

TEST_F(SemanticAnalysisTests, ArrayAccessOutOfBoundsFail) {
    std::istringstream iss(R"(
        int main() {
        int i[3];
        int j;
        j = i[3];
        return 0;
}
)");

    CallThrow(iss, "5:14: Array assign out of bounds");
}
