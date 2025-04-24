#pragma once

#include <deque>
#include <iostream>
#include <stack>
#include <string>

#include "i_visitor.h"

class PrintVisitor : public IASTVisitor {
   public:
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

    void Print(std::ostream& os);

   private:
    std::stack<std::string> result_stack_;
    std::deque<std::string> result_deque_;
};