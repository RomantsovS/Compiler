#include "interpreter.h"

#include <algorithm>
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
#include "ir.h"

int Interpreter::Exec() {
    auto obj = Eval(ir_.GetAST());

    auto number_ptr = obj.TryAs<Number>();
    if (number_ptr) return number_ptr->GetValue();

    return 1;
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::Program> node) {
    for (auto global : node->globals) {
        Eval(global);
    }
    auto iter = std::find_if(
        node->functions.begin(), node->functions.end(),
        [](std::shared_ptr<AST::ASTNode> function) {
            auto func = std::dynamic_pointer_cast<AST::Function>(function);
            return func && func->name == "main";
        });
    if (iter == node->functions.end()) {
        Error(node.get(), "Main function isn't found");
    }

    return Eval(*iter);
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::Function> node,
                               std::vector<ObjectHolder> args) {
    for (size_t i = 0; i < node->args.size(); ++i) {
        variables[node->args[i].name] = args[i];
    }
    for (auto stmt : node->body) {
        Eval(stmt);
    }
    return {};
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::Print> node) {
    auto obj_holder = Eval(node->expr);
    if (obj_holder) obj_holder->Print(os_);
    os_ << '\n';
    return {};
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::FunCall> node) {
    std::vector<ObjectHolder> args;
    for (size_t i = 0; i < node->args.size(); ++i) {
        args.push_back(Eval(node->args[i]));
    }

    auto func = ir_.GetFunction(node->name);
    return Eval(func, args);
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::Return> node) {
    return Eval(node->expr);
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::ArithOp> node) {
    auto lhs = Eval(node->lhs);
    auto rhs = Eval(node->rhs);

    if (node->op == "+") {
        return ObjectHolder::Own(ValueObject(lhs.TryAs<Number>()->GetValue() +
                                             rhs.TryAs<Number>()->GetValue()));
    } else if (node->op == "-") {
        return ObjectHolder::Own(ValueObject(lhs.TryAs<Number>()->GetValue() -
                                             rhs.TryAs<Number>()->GetValue()));
    } else if (node->op == "*") {
        return ObjectHolder::Own(ValueObject(lhs.TryAs<Number>()->GetValue() *
                                             rhs.TryAs<Number>()->GetValue()));
    } else if (node->op == "/") {
        return ObjectHolder::Own(ValueObject(lhs.TryAs<Number>()->GetValue() /
                                             rhs.TryAs<Number>()->GetValue()));
    } else {
        Error(node.get(), "Unknown arith op: ", node->op);
    }
    return {};
}

/*
void Interpreter::visit(AST::IfThenElse* node) {
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

void Interpreter::visit(AST::LogicOp* node) {
    // os_ << "(";
    node->lhs->accept(this);
    // os_ << ")";
    os_ << " " << node->op << " ";
    // os_ << "(";
    node->rhs->accept(this);
    // os_ << ")";
}

void Interpreter::visit(AST::VarDef* node) {}

void Interpreter::visit(AST::Var* node) { os_ << node->name; }

void Interpreter::visit(AST::Integer* node) { os_ << node->value; }
*/
ObjectHolder Interpreter::Eval(std::shared_ptr<AST::Assign> node) {
    variables[node->var] = Eval(node->expr);
    return variables[node->var];
}
/*
void Interpreter::visit(AST::While* node) {
    os_ << "while (";
    node->condition->accept(this);
    os_ << ") {\n";
    for (size_t i = 0; i < node->body.size(); ++i) {
        if (i > 0) os_ << "\n";
        node->body[i]->accept(this);
    }
    os_ << "\n}\n";
}

void Interpreter::visit(AST::StringLiteral* node) {
    os_ << "\"" << node->value << "\"";
}

void Interpreter::visit(AST::BoolLiteral* node) {
    os_ << (node->value ? "true" : "false");
}
*/
ObjectHolder Interpreter::Eval(std::shared_ptr<AST::VarDef> node) {
    variables[node->name] = {};
    return variables[node->name];
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::Var> node) {
    auto iter = variables.find(node->name);
    if (iter == variables.end()) {
        Error(node.get(), "Variable ", node->name, " undefined");
    }
    if (!iter->second) {
        Error(node.get(), "Variable ", node->name, " uninitialized");
    }
    return iter->second;
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::ArrayDeclaration> node) {
    variables[node->name] = ObjectHolder::Own(ArrayObject());
    return variables[node->name];
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::ArrayAccess> node) {
    auto index_obj = Eval(node->index);
    auto index_ptr = index_obj.TryAs<Number>();
    if (!index_ptr) {
        Error(node.get(), "Index expr is not int");
    }
    auto iter = variables.find(node->name);
    if (iter == variables.end()) {
        Error(node.get(), "Variable ", node->name, " undefined");
    }
    auto arr_obj = iter->second.TryAs<ArrayObject>();
    if (!arr_obj) {
        Error(node.get(), "Not array object");
    }
    return arr_obj->GetObject(index_ptr->GetValue());
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::ArrayAssignment> node) {
    auto index_obj = Eval(node->index);
    auto index_ptr = index_obj.TryAs<Number>();
    if (!index_ptr) {
        Error(node.get(), "Index expr is not int");
    }
    auto iter = variables.find(node->name);
    if (iter == variables.end()) {
        Error(node.get(), "Variable ", node->name, " undefined");
    }
    auto arr_obj = iter->second.TryAs<ArrayObject>();
    if (!arr_obj) {
        Error(node.get(), "Not array object");
    }
    arr_obj->SetObject(index_ptr->GetValue(), Eval(node->expr));
    return variables[node->name];
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::ASTNode> node) {
    if (auto prog = std::dynamic_pointer_cast<AST::Program>(node)) {
        return Eval(prog);
    } else if (auto func = std::dynamic_pointer_cast<AST::Function>(node)) {
        return Eval(func, {});
    } else if (auto fun_call = std::dynamic_pointer_cast<AST::FunCall>(node)) {
        return Eval(fun_call);
    } else if (auto ret = std::dynamic_pointer_cast<AST::Return>(node)) {
        return Eval(ret);
    } else if (auto int_lit = std::dynamic_pointer_cast<AST::Integer>(node)) {
        return ObjectHolder::Own(ValueObject(int_lit->value));
    } else if (auto str_lit =
                   std::dynamic_pointer_cast<AST::StringLiteral>(node)) {
        return ObjectHolder::Own(ValueObject(str_lit->value));
    } else if (auto b_lit = std::dynamic_pointer_cast<AST::BoolLiteral>(node)) {
        return ObjectHolder::Own(ValueObject(b_lit->value));
    } else if (auto var_def = std::dynamic_pointer_cast<AST::VarDef>(node)) {
        return Eval(var_def);
    } else if (auto var = std::dynamic_pointer_cast<AST::Var>(node)) {
        return Eval(var);
    } else if (auto arr_decl =
                   std::dynamic_pointer_cast<AST::ArrayDeclaration>(node)) {
        return Eval(arr_decl);
    } else if (auto arr_ac =
                   std::dynamic_pointer_cast<AST::ArrayAccess>(node)) {
        return Eval(arr_ac);
    } else if (auto arr_assign =
                   std::dynamic_pointer_cast<AST::ArrayAssignment>(node)) {
        return Eval(arr_assign);
    } else if (auto print = std::dynamic_pointer_cast<AST::Print>(node)) {
        return Eval(print);
    } else if (auto assign = std::dynamic_pointer_cast<AST::Assign>(node)) {
        return Eval(assign);
    } else if (auto ar_op = std::dynamic_pointer_cast<AST::ArithOp>(node)) {
        return Eval(ar_op);
    }

    Error(node.get(), "unknown node");
    return {};
}
