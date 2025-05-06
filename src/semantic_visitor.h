#pragma once

#include <algorithm>
#include <cassert>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast/ast.h"
#include "ast/type.h"
#include "call_stack.h"
#include "i_visitor.h"
#include "ir.h"

struct SymbolEntry {
    AST::Type type;
    AST::Location loc;
    std::vector<AST::Type> params;
};

class SemanticVisitor : public IASTVisitor {
   public:
    SemanticVisitor(IR* ir) : ir_(ir) {}

    void visit(AST::ArithOp* node) override;
    void visit(AST::ArrayAccess* node) override;
    void visit(AST::ArrayAssignment* node) override;
    void visit(AST::ArrayDeclaration* node) override;
    void visit(AST::Assign* node) override;
    void visit(AST::BoolLiteral* node) override;
    void visit(AST::FunCall* node) override;
    void visit(AST::Function* node) override;
    void visit(AST::IfThenElse* node) override;
    void visit(AST::Integer* node) override;
    void visit(AST::LogicOp* node) override;
    void visit(AST::Print* node) override;
    void visit(AST::Program* node) override;
    void visit(AST::Rand* node) override;
    void visit(AST::Return* node) override;
    void visit(AST::StringLiteral* node) override;
    void visit(AST::Var* node) override;
    void visit(AST::VarDef* node) override;
    void visit(AST::While* node) override;

   private:
    template <typename... Args>
    void Error(AST::ASTNode* node, Args... args) {
        std::ostringstream oss;
        oss << node->loc;
        oss << ": ";
        (oss << ... << args);
        throw std::runtime_error(oss.str());
    }

    IR* ir_;

    CallStack<SymbolEntry> symtable;
};