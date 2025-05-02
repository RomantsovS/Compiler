#include "semantic_visitor.h"

#include <cassert>
#include <sstream>
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

void SemanticVisitor::visit(AST::Program* node) {
    for (auto global : node->globals) {
        global->accept(this);
    }
    for (auto function : node->functions) {
        function->accept(this);
    }
}

void SemanticVisitor::visit(AST::Function* node) {
    if (!symtable.Declare(node->name, {node->return_type})) {
        Error(node, "Redeclaration of " + node->name);
    }
    symtable.PushScope();

    for (size_t i = 0; i < node->args.size(); ++i) {
        if (!symtable.Declare(node->args[i].name, {node->args[i].type})) {
            Error(node, "Redeclaration of " + node->args[i].name);
        }
    }
    for (auto stmt : node->body) {
        stmt->accept(this);
    }
}

void SemanticVisitor::visit(AST::Print* node) { node->st->accept(this); }

void SemanticVisitor::visit(AST::FunCall* node) {
    auto* entry = symtable.Find(node->name);
    if (!entry) {
        Error(node, "Undeclared func " + node->name);
    }
    for (size_t i = 0; i < node->args.size(); ++i) {
        node->args[i]->accept(this);
    }
}

void SemanticVisitor::visit(AST::IfThenElse* node) {
    node->condition->accept(this);
    node->then_branch->accept(this);
    if (node->else_branch) {
        node->else_branch->accept(this);
    }
}

void SemanticVisitor::visit(AST::LogicOp* node) {
    node->lhs->accept(this);
    node->rhs->accept(this);
}

void SemanticVisitor::visit(AST::Return* node) {
    node->statement->accept(this);
}

void SemanticVisitor::visit(AST::ArithOp* node) {
    node->lhs->accept(this);
    node->rhs->accept(this);

    if (node->lhs->type != node->rhs->type) {
        Error(node, "Type mismatch: cannot perform " + node->op + " for " +
                        node->lhs->type.to_string() + " and " +
                        node->rhs->type.to_string());
    }

    node->type = node->lhs->type;
}

void SemanticVisitor::visit(AST::VarDef* node) {
    if (!symtable.Declare(node->name, {node->type})) {
        Error(node, "Redeclaration of " + node->name);
    }
}

void SemanticVisitor::visit(AST::Var* node) {
    auto* entry = symtable.Find(node->name);
    if (!entry) {
        Error(node, "Undeclared variable " + node->name);
    }
    node->type = entry->type;
}

void SemanticVisitor::visit(AST::Integer* node) {}

void SemanticVisitor::visit(AST::Assign* node) {
    auto* entry = symtable.Find(node->var);
    if (!entry) {
        Error(node, "Undeclared variable " + node->var);
    }
    node->expr->accept(this);
    if (entry->type != node->expr->type) {
        Error(node, "Type mismatch: cannot assign " +
                        node->expr->type.to_string() + " to " + node->var);
    }
}

void SemanticVisitor::visit(AST::While* node) {
    node->condition->accept(this);
    for (size_t i = 0; i < node->body.size(); ++i) {
        node->body[i]->accept(this);
    }
}

void SemanticVisitor::visit(AST::StringLiteral* node) {}

void SemanticVisitor::visit(AST::BoolLiteral* node) {}

void SemanticVisitor::visit(AST::ArrayDeclaration* node) {
    if (!symtable.Declare(node->name, {node->type})) {
        Error(node, "Redeclaration of " + node->name);
    }
}

void SemanticVisitor::visit(AST::ArrayAccess* node) {
    auto* entry = symtable.Find(node->name);
    if (!entry) {
        Error(node, "Undeclared array variable " + node->name);
    }
    node->index->accept(this);
}

void SemanticVisitor::visit(AST::ArrayAssignment* node) {
    auto* entry = symtable.Find(node->name);
    if (!entry) {
        Error(node, "Undeclared array variable " + node->name);
    }
    node->index->accept(this);
    node->value->accept(this);
}

void SemanticVisitor::Error(AST::ASTNode* node, std::string_view msg) {
    std::ostringstream oss;
    oss << node->loc;
    oss << ": " << msg;
    throw std::runtime_error(oss.str());
}