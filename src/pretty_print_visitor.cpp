#include "pretty_print_visitor.h"

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
#include "ast/rand.h"
#include "ast/return.h"
#include "ast/string_literal.h"
#include "ast/var.h"
#include "ast/while.h"

std::string ToString(const AST::Type type) {
    switch (type.base) {
        case AST::BaseType::Void:
            return "void";
        case AST::BaseType::Int:
            return "int";
        case AST::BaseType::Bool:
            return "bool";
        default:
            return "Unknown";
    }
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

void PrettyPrintVisitor::visit(AST::ArrayAccess* node) {
    os_ << node->name << "[";
    node->index->accept(this);
    os_ << "]";
}

void PrettyPrintVisitor::visit(AST::ArrayAssignment* node) {
    os_ << node->name << "[";
    node->index->accept(this);
    os_ << "] = ";
    node->expr->accept(this);
    os_ << ";";
}

void PrettyPrintVisitor::visit(AST::ArrayDeclaration* node) {
    std::cout << ToString(node->type) << " " << node->name << "["
              << node->type.array_size << "];";
}

void PrettyPrintVisitor::visit(AST::Assign* node) {
    os_ << node->var << " = ";
    node->expr->accept(this);
    os_ << ";";
}

void PrettyPrintVisitor::visit(AST::BoolLiteral* node) {
    os_ << (node->value ? "true" : "false");
}

void PrettyPrintVisitor::visit(AST::FunCall* node) {
    os_ << node->name << "(";
    for (size_t i = 0; i < node->args.size(); ++i) {
        if (i > 0) os_ << ", ";
        node->args[i]->accept(this);
    }
    os_ << ");";
}

void PrettyPrintVisitor::visit(AST::Function* node) {
    os_ << ToString(node->return_type) << " " << node->name << "(";
    for (size_t i = 0; i < node->args.size(); ++i) {
        if (i > 0) os_ << ", ";
        os_ << ToString(node->args[i].type) << " " << node->args[i].name;
    }
    os_ << ") {\n";
    for (auto stmt : node->body) {
        stmt->accept(this);
        os_ << "\n";
    }
    os_ << "}";
}

void PrettyPrintVisitor::visit(AST::IfThenElse* node) {
    os_ << "if (";
    node->condition->accept(this);
    os_ << ") {\n";
    for (size_t i = 0; i < node->then_branch.size(); ++i) {
        if (i > 0) os_ << "\n";
        node->then_branch[i]->accept(this);
    }
    os_ << "\n}";
    if (!node->else_branch.empty()) {
        os_ << " else {\n";
        for (size_t i = 0; i < node->else_branch.size(); ++i) {
            if (i > 0) os_ << "\n";
            node->else_branch[i]->accept(this);
        }
        os_ << "\n}";
    }
}

void PrettyPrintVisitor::visit(AST::Integer* node) { os_ << node->value; }

void PrettyPrintVisitor::visit(AST::LogicOp* node) {
    node->lhs->accept(this);
    os_ << " " << node->op << " ";
    node->rhs->accept(this);
}

void PrettyPrintVisitor::visit(AST::Print* node) {
    os_ << "std::cout << (";
    node->expr->accept(this);
    os_ << ");";
}

void PrettyPrintVisitor::visit(AST::Program* node) {
    os_ << "#include <iostream>\n\n";

    for (auto function : node->globals) {
        function->accept(this);
        os_ << "\n\n";
    }
}

void PrettyPrintVisitor::visit(AST::Rand* node) { os_ << "rand()"; }

void PrettyPrintVisitor::visit(AST::Return* node) {
    os_ << "return ";
    node->expr->accept(this);
    os_ << ";";
}

void PrettyPrintVisitor::visit(AST::StringLiteral* node) {
    os_ << "\"" << node->value << "\"";
}

void PrettyPrintVisitor::visit(AST::Var* node) { os_ << node->name; }

void PrettyPrintVisitor::visit(AST::VarDef* node) {
    os_ << ToString(node->type) << " " << node->name << ";";
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
