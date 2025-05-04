#include "print_visitor.h"

#include <cassert>
#include <string>

#include "ast/arithmetic_op.h"
#include "ast/array.h"
#include "ast/assign.h"
#include "ast/bool_literal.h"
#include "ast/fun_call.h"
#include "ast/function.h"
#include "ast/if_then_else.h"
#include "ast/integer.h"
#include "ast/logic_op.h"
#include "ast/print.h"
#include "ast/program.h"
#include "ast/return.h"
#include "ast/string_literal.h"
#include "ast/var.h"
#include "ast/while.h"

void PrintVisitor::visit(AST::Program* node) {
    PrintIndent();
    PrintLoc(node);
    os_ << "Program: \n";
    IndentRAII indent_raii(indent_);

    for (auto global : node->globals) {
        global->accept(this);
        os_ << "\n";
    }
    for (auto function : node->functions) {
        function->accept(this);
        os_ << "\n";
    }
}

void PrintVisitor::visit(AST::Function* node) {
    PrintIndent();
    PrintLoc(node);
    os_ << "Function: " << node->return_type << " " << node->name << "(";
    for (size_t i = 0; i < node->args.size(); ++i) {
        if (i > 0) os_ << ", ";
        os_ << node->args[i].type << " " << node->args[i].name;
    }
    os_ << ") {}\n";
    IndentRAII indent_raii(indent_);

    for (auto stmt : node->body) {
        stmt->accept(this);
        os_ << "\n";
    }
}

void PrintVisitor::visit(AST::Print* node) {
    PrintIndent();
    PrintLoc(node);
    os_ << "Print: (expr)\n";
    IndentRAII indent_raii(indent_);

    node->expr->accept(this);
}

void PrintVisitor::visit(AST::FunCall* node) {
    PrintIndent();
    PrintLoc(node);
    os_ << "FunCall: " << node->name << "(args)\n";
    IndentRAII indent_raii(indent_);

    for (size_t i = 0; i < node->args.size(); ++i) {
        node->args[i]->accept(this);
        os_ << "\n";
    }
}

void PrintVisitor::visit(AST::IfThenElse* node) {
    PrintIndent();
    PrintLoc(node);
    os_ << "IfThenElse: (expr) { stmt } else { stmt }\n";
    IndentRAII indent_raii(indent_);

    node->condition->accept(this);
    os_ << "\n";
    node->then_branch->accept(this);
    os_ << "\n";
    if (node->else_branch) {
        node->else_branch->accept(this);
    } else {
        os_ << "<No else>";
    }
}

void PrintVisitor::visit(AST::LogicOp* node) {
    PrintIndent();
    PrintLoc(node);
    os_ << "LogicOp: (expr)" << " " << node->op << " (expr)\n";
    IndentRAII indent_raii(indent_);

    node->lhs->accept(this);
    os_ << "\n";
    node->rhs->accept(this);
}

void PrintVisitor::visit(AST::Return* node) {
    PrintIndent();
    PrintLoc(node);
    os_ << "Return:\n";
    IndentRAII indent_raii(indent_);

    node->expr->accept(this);
}

void PrintVisitor::visit(AST::ArithOp* node) {
    PrintIndent();
    PrintLoc(node);
    os_ << "ArithOp: (expr)" << " " << node->op << " (expr)\n";
    IndentRAII indent_raii(indent_);

    node->lhs->accept(this);
    os_ << "\n";
    node->rhs->accept(this);
}

void PrintVisitor::visit(AST::VarDef* node) {
    PrintIndent();
    PrintLoc(node);
    os_ << "VarDef: " << node->type << " " << node->name;
}

void PrintVisitor::visit(AST::Var* node) {
    PrintIndent();
    PrintLoc(node);
    os_ << "Var: " << node->name;
}

void PrintVisitor::visit(AST::Integer* node) {
    PrintIndent();
    PrintLoc(node);
    os_ << "Integer: " << node->value;
}

void PrintVisitor::visit(AST::Assign* node) {
    PrintIndent();
    PrintLoc(node);
    os_ << "Assign: " << node->var << " = (expr)\n";
    IndentRAII indent_raii(indent_);

    node->expr->accept(this);
}

void PrintVisitor::visit(AST::While* node) {
    PrintIndent();
    PrintLoc(node);
    os_ << "While: (expr) { stmt list }\n";
    IndentRAII indent_raii(indent_);

    node->condition->accept(this);
    os_ << "\n";

    for (size_t i = 0; i < node->body.size(); ++i) {
        node->body[i]->accept(this);
        os_ << "\n";
    }
}

void PrintVisitor::visit(AST::StringLiteral* node) {
    PrintIndent();
    PrintLoc(node);
    os_ << "StringLiteral(" << node->value << ")";
}

void PrintVisitor::visit(AST::BoolLiteral* node) {
    PrintIndent();
    PrintLoc(node);
    os_ << "BoolLiteral(" << (node->value ? "true" : "false") << ")";
}

void PrintVisitor::visit(AST::ArrayDeclaration* node) {
    PrintIndent();
    PrintLoc(node);
    std::cout << "ArrayDeclaration: " << node->type << " " << node->name << "["
              << node->type.array_size << "];";
}

void PrintVisitor::visit(AST::ArrayAccess* node) {
    PrintIndent();
    PrintLoc(node);
    os_ << "ArrayAccess: " << node->name << "[expr]\n";
    IndentRAII indent_raii(indent_);

    node->index->accept(this);
}

void PrintVisitor::visit(AST::ArrayAssignment* node) {
    PrintIndent();
    PrintLoc(node);
    os_ << "ArrayAssignment: " << node->name << "[expr] = expr\n";
    IndentRAII indent_raii(indent_);

    node->index->accept(this);
    os_ << '\n';
    node->expr->accept(this);
}

void PrintVisitor::PrintIndent() { os_ << std::string(indent_, ' '); }

inline void PrintVisitor::PrintLoc(AST::ASTNode* node) {
    os_ << "(" << node->loc << ")";
}
