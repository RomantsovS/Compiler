#include "pretty_print_visitor.h"

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

void PrettyPrintVisitor::visit(AST::Program* node) {
    os_ << "#include <iostream>\n\n";

    for (auto global : node->globals) {
        global->accept(this);
        os_ << "\n";
    }
    os_ << "\n";
    for (auto function : node->functions) {
        function->accept(this);
        os_ << "\n\n";
    }
}

void PrettyPrintVisitor::visit(AST::Function* node) {
    os_ << node->return_type.to_string() << " " << node->name << "(";
    for (size_t i = 0; i < node->args.size(); ++i) {
        if (i > 0) os_ << ", ";
        os_ << node->args[i].type.to_string() << " " << node->args[i].name;
    }
    os_ << ") {\n";
    for (auto stmt : node->body) {
        stmt->accept(this);
        os_ << "\n";
    }
    os_ << "}";
}

void PrettyPrintVisitor::visit(AST::Print* node) {
    os_ << "std::cout << (";
    node->st->accept(this);
    os_ << ");";
}

void PrettyPrintVisitor::visit(AST::FunCall* node) {
    os_ << node->name << "(";
    for (size_t i = 0; i < node->args.size(); ++i) {
        if (i > 0) os_ << ", ";
        node->args[i]->accept(this);
    }
    os_ << ");";
}

void PrettyPrintVisitor::visit(AST::IfThenElse* node) {
    os_ << "if (";
    node->condition->accept(this);
    os_ << ") {\n";
    node->then_branch->accept(this);
    os_ << "\n}";
    if (node->else_branch) {
        os_ << " else {\n";
        node->else_branch->accept(this);
        os_ << "\n}";
    }
}

void PrettyPrintVisitor::visit(AST::LogicOp* node) {
    // os_ << "(";
    node->lhs->accept(this);
    // os_ << ")";
    os_ << " " << node->op << " ";
    // os_ << "(";
    node->rhs->accept(this);
    // os_ << ")";
}

void PrettyPrintVisitor::visit(AST::Return* node) {
    os_ << "return ";
    node->statement->accept(this);
    os_ << ";";
}

void PrettyPrintVisitor::visit(AST::ArithOp* node) {
    os_ << "(";
    node->lhs->accept(this);
    os_ << ")";
    os_ << " " << node->op << " ";
    os_ << "(";
    node->rhs->accept(this);
    os_ << ")";
}

void PrettyPrintVisitor::visit(AST::VarDef* node) {
    os_ << node->type.to_string() << " " << node->name << ";";
}

void PrettyPrintVisitor::visit(AST::Var* node) { os_ << node->name; }

void PrettyPrintVisitor::visit(AST::Integer* node) { os_ << node->val; }

void PrettyPrintVisitor::visit(AST::Assign* node) {
    os_ << node->var << " = ";
    node->expr->accept(this);
    os_ << ";";
}

void PrettyPrintVisitor::visit(AST::While* node) {
    os_ << "while (";
    node->condition->accept(this);
    os_ << ") {\n";
    for (size_t i = 0; i < node->body.size(); ++i) {
        if (i > 0) os_ << "\n";
        node->body[i]->accept(this);
    }
    os_ << "\n}\n";
}

void PrettyPrintVisitor::visit(AST::StringLiteral* node) {
    os_ << "\"" << node->value << "\"";
}

void PrettyPrintVisitor::visit(AST::BoolLiteral* node) {
    os_ << (node->value ? "true" : "false");
}

void PrettyPrintVisitor::visit(AST::ArrayDeclaration* node) {
    std::cout << node->type.to_string() << " " << node->name << "["
              << node->type.array_size << "];";
}

void PrettyPrintVisitor::visit(AST::ArrayAccess* node) {
    os_ << node->name << "[";
    node->index->accept(this);
    os_ << "]";
}

void PrettyPrintVisitor::visit(AST::ArrayAssignment* node) {
    os_ << node->name << "[";
    node->index->accept(this);
    os_ << "] = ";
    node->value->accept(this);
    os_ << ";";
}
