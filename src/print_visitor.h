#pragma once

#include <iostream>
#include <string>

#include "i_visitor.h"

class PrintVisitor : public IASTVisitor {
   public:
    PrintVisitor(std::ostream& os) : os_(os) {}

    void visit(AST::Program* node) override;
    void visit(AST::Function* node) override;
    void visit(AST::Print* node) override;
    void visit(AST::FunCall* node) override;
    void visit(AST::IfThenElse* node) override;
    void visit(AST::LogicOp* node) override;
    void visit(AST::Return* node) override;
    void visit(AST::ArithOp* node) override;
    void visit(AST::VarDef* node) override;
    void visit(AST::Var* node) override;
    void visit(AST::Integer* node) override;
    void visit(AST::Assign* node) override;
    void visit(AST::While* node) override;
    void visit(AST::StringLiteral* node) override;
    void visit(AST::BoolLiteral* node) override;
    void visit(AST::ArrayDeclaration* node) override;
    void visit(AST::ArrayAccess* node) override;
    void visit(AST::ArrayAssignment* node) override;

   private:
    void PrintIndent();
    void PrintLoc(AST::ASTNode* node);

    class IndentRAII {
       public:
        IndentRAII(int& indent) : indent_(indent) { indent_ += indent_size_; }
        ~IndentRAII() { indent_ -= indent_size_; }

       private:
        int& indent_;
        const int indent_size_ = 2;
    };

    std::ostream& os_;
    int indent_ = 0;
};