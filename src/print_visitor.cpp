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
    assert(result_stack_.size() >= node->globals.size());
    std::string str;

    std::stack<std::string> temp_st;
    for (size_t i = 0; i < node->globals.size(); ++i) {
        temp_st.push(result_stack_.top());
        result_stack_.pop();
    }
    for (size_t i = 0; i < node->globals.size(); ++i) {
        if (i > 0) str += "\n";
        str += temp_st.top();
        temp_st.pop();
    }
    str += "\n";
    result_deque_.push_front(str);
}

void PrintVisitor::visit(AST::Function* node) {
    auto str = node->return_type.to_string() + " " + node->name + "(";
    for (size_t i = 0; i < node->args.size(); ++i) {
        if (i > 0) str += ", ";
        str += node->args[i].type.to_string() + " " + node->args[i].name;
    }
    str += ") {\n";
    assert(result_stack_.size() >= node->body.size());
    std::stack<std::string> temp_st;
    for (size_t i = 0; i < node->body.size(); ++i) {
        temp_st.push(result_stack_.top());
        result_stack_.pop();
    }
    for (size_t i = 0; i < node->body.size(); ++i) {
        if (i > 0) str += "\n";
        str += temp_st.top();
        temp_st.pop();
    }
    str += "\n}";
    result_deque_.push_back(str);
}

void PrintVisitor::visit(AST::Print* node) {
    assert(!result_stack_.empty());
    auto a = result_stack_.top();
    result_stack_.pop();
    result_stack_.push("std::cout << (" + a + ");");
}

void PrintVisitor::visit(AST::FunCall* node) {
    assert(result_stack_.size() >= node->args.size());
    std::stack<std::string> temp_st;
    for (size_t i = 0; i < node->args.size(); ++i) {
        temp_st.push(result_stack_.top());
        result_stack_.pop();
    }

    auto str = node->name + "(";
    for (size_t i = 0; i < node->args.size(); ++i) {
        if (i > 0) str += ", ";
        str += temp_st.top();
        temp_st.pop();
    }
    str += ")";
    result_stack_.push(str);
}

void PrintVisitor::visit(AST::IfThenElse* node) {
    assert(result_stack_.size() >= 3);
    auto a = result_stack_.top();
    result_stack_.pop();
    auto b = result_stack_.top();
    result_stack_.pop();
    auto c = result_stack_.top();
    result_stack_.pop();
    result_stack_.push(std::string("if (") + c + ") { " + b + " } else { " + a +
                       " }");
}

void PrintVisitor::visit(AST::LogicOp* node) {
    assert(result_stack_.size() >= 2);
    auto a = result_stack_.top();
    result_stack_.pop();
    auto b = result_stack_.top();
    result_stack_.pop();
    result_stack_.push(b + " " + node->op + " " + a);
}

void PrintVisitor::visit(AST::Return* node) {
    assert(!result_stack_.empty());
    auto a = result_stack_.top();
    result_stack_.pop();
    result_stack_.push("return " + a + ";");
}

void PrintVisitor::visit(AST::ArithOp* node) {
    assert(result_stack_.size() >= 2);
    auto a = result_stack_.top();
    result_stack_.pop();
    auto b = result_stack_.top();
    result_stack_.pop();
    result_stack_.push("(" + b + ") " + node->op + " (" + a + ")");
}

void PrintVisitor::visit(AST::VarDef* node) {
    result_stack_.push(node->type.to_string() + " " + node->name + ";");
}

void PrintVisitor::visit(AST::Var* node) { result_stack_.push(node->name); }

void PrintVisitor::visit(AST::Integer* node) {
    result_stack_.push(std::to_string(node->val));
}

void PrintVisitor::visit(AST::Assign* node) {
    assert(result_stack_.size() >= 1);
    auto expr = result_stack_.top();
    result_stack_.pop();
    result_stack_.push(node->var + " = " + expr + ";");
}

void PrintVisitor::visit(AST::While* node) {
    assert(result_stack_.size() >= node->body.size() + 1);
    auto cond = result_stack_.top();
    result_stack_.pop();
    std::stack<std::string> temp_st;
    for (size_t i = 0; i < node->body.size(); ++i) {
        temp_st.push(result_stack_.top());
        result_stack_.pop();
    }
    auto str = "while (" + cond + ") {\n";
    for (size_t i = 0; i < node->body.size(); ++i) {
        if (i > 0) str += "\n";
        str += temp_st.top();
        temp_st.pop();
    }
    str += "\n}\n";
    result_stack_.push(str);
}

void PrintVisitor::visit(AST::StringLiteral* node) {
    result_stack_.push("\"" + node->value + "\"");
}

void PrintVisitor::visit(AST::BoolLiteral* node) {
    result_stack_.push(node->value ? "true" : "false");
}

void PrintVisitor::visit(AST::ArrayDeclaration* node) {
    result_stack_.push(node->type.to_string() + " " + node->name + "[" +
                       std::to_string(node->type.array_size) + "];");
}

void PrintVisitor::visit(AST::ArrayAccess* node) {
    assert(result_stack_.size() >= 1);
    auto expr = result_stack_.top();
    result_stack_.pop();
    result_stack_.push(node->name + "[" + expr + "]");
}

void PrintVisitor::visit(AST::ArrayAssignment* node) {
    assert(result_stack_.size() >= 2);
    auto value = result_stack_.top();
    result_stack_.pop();
    auto expr = result_stack_.top();
    result_stack_.pop();
    result_stack_.push(node->name + "[" + expr + "] = " + value + ";");
}

void PrintVisitor::Print(std::ostream& os) {
    os << "#include <iostream>\n\n";

    while (!result_deque_.empty()) {
        os << result_deque_.front() << "\n\n";
        result_deque_.pop_front();
    }
}