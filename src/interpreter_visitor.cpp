#include "interpreter_visitor.h"

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

void InterpreterVisitor::visit(AST::Program* node) {
    for (auto global : node->globals) {
        global->accept(this);
    }
    for (auto function : node->functions) {
        function->accept(this);
    }
}

void InterpreterVisitor::visit(AST::Function* node) {
    for (size_t i = 0; i < node->args.size(); ++i) {
    }
    for (auto stmt : node->body) {
        stmt->accept(this);
    }
}

void InterpreterVisitor::visit(AST::Print* node) {
    auto obj_holder = Eval(node->expr);
    obj_holder->Print(os_);
    os_ << '\n';
}

void InterpreterVisitor::visit(AST::FunCall* node) {
    os_ << node->name << "(";
    for (size_t i = 0; i < node->args.size(); ++i) {
        if (i > 0) os_ << ", ";
        node->args[i]->accept(this);
    }
    os_ << ");";
}

void InterpreterVisitor::visit(AST::IfThenElse* node) {
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

void InterpreterVisitor::visit(AST::LogicOp* node) {
    // os_ << "(";
    node->lhs->accept(this);
    // os_ << ")";
    os_ << " " << node->op << " ";
    // os_ << "(";
    node->rhs->accept(this);
    // os_ << ")";
}

void InterpreterVisitor::visit(AST::Return* node) {
    os_ << "return ";
    node->expr->accept(this);
    os_ << ";";
}

void InterpreterVisitor::visit(AST::ArithOp* node) {
    os_ << "(";
    node->lhs->accept(this);
    os_ << ")";
    os_ << " " << node->op << " ";
    os_ << "(";
    node->rhs->accept(this);
    os_ << ")";
}

void InterpreterVisitor::visit(AST::VarDef* node) {}

void InterpreterVisitor::visit(AST::Var* node) { os_ << node->name; }

void InterpreterVisitor::visit(AST::Integer* node) { os_ << node->value; }

void InterpreterVisitor::visit(AST::Assign* node) {
    variables[node->var] = Eval(node->expr);
}

void InterpreterVisitor::visit(AST::While* node) {
    os_ << "while (";
    node->condition->accept(this);
    os_ << ") {\n";
    for (size_t i = 0; i < node->body.size(); ++i) {
        if (i > 0) os_ << "\n";
        node->body[i]->accept(this);
    }
    os_ << "\n}\n";
}

void InterpreterVisitor::visit(AST::StringLiteral* node) {
    os_ << "\"" << node->value << "\"";
}

void InterpreterVisitor::visit(AST::BoolLiteral* node) {
    os_ << (node->value ? "true" : "false");
}

void InterpreterVisitor::visit(AST::ArrayDeclaration* node) {}

void InterpreterVisitor::visit(AST::ArrayAccess* node) {
    os_ << node->name << "[";
    node->index->accept(this);
    os_ << "]";
}

void InterpreterVisitor::visit(AST::ArrayAssignment* node) {
    auto index_obj = Eval(node->index);
    auto index_ptr = index_obj.TryAs<Number>();
    if (!index_ptr) {
        throw std::runtime_error("Index expr is not int");
    }
    ArrayObject arr;
    auto val = Eval(node->expr);
    arr.SetObject(index_ptr->GetValue(), val);
    variables[node->name] = ObjectHolder::Own(std::move(arr));
}

ObjectHolder InterpreterVisitor::Eval(std::shared_ptr<AST::ASTNode> node) {
    if (auto int_lit = std::dynamic_pointer_cast<AST::Integer>(node))
        return ObjectHolder::Own(ValueObject(int_lit->value));
    if (auto str_lit = std::dynamic_pointer_cast<AST::StringLiteral>(node))
        return ObjectHolder::Own(ValueObject(str_lit->value));
    if (auto var = std::dynamic_pointer_cast<AST::Var>(node)) {
        return Eval(var);
    }
    if (auto arr_ac = std::dynamic_pointer_cast<AST::ArrayAccess>(node)) {
        return Eval(arr_ac);
    }

    throw std::runtime_error("unknown node");
}

ObjectHolder InterpreterVisitor::Eval(std::shared_ptr<AST::Var> node) {
    return variables.at(node->name);
}

ObjectHolder InterpreterVisitor::Eval(std::shared_ptr<AST::ArrayAccess> node) {
    auto index_obj = Eval(node->index);
    auto index_ptr = index_obj.TryAs<Number>();
    if (!index_ptr) {
        throw std::runtime_error("Index expr is not int");
    }
    auto arr_holder = variables.at(node->name);
    return arr_holder.TryAs<ArrayObject>()->GetObject(index_ptr->GetValue());
}