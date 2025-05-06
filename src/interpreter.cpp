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
    } else if (node->op == "%") {
        return ObjectHolder::Own(ValueObject(lhs.TryAs<Number>()->GetValue() %
                                             rhs.TryAs<Number>()->GetValue()));
    } else {
        Error(node.get(), "Unknown arith op: ", node->op);
    }
    return ObjectHolder::None();
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::ArrayAccess> node) {
    auto index_obj = Eval(node->index);
    auto index_ptr = index_obj.TryAs<Number>();
    if (!index_ptr) {
        Error(node.get(), "Index expr is not int");
    }
    auto* var = call_stack.Find(node->name);
    if (!var) {
        Error(node.get(), "Variable ", node->name, " undefined");
    }
    auto arr_obj = var->TryAs<ArrayObject>();
    if (!arr_obj) {
        Error(node.get(), "Not array object");
    }
    try {
        return arr_obj->GetObject(index_ptr->GetValue());
    } catch (std::exception& ex) {
        Error(node.get(), ex.what());
    }
    return ObjectHolder::None();
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::ArrayDeclaration> node) {
    call_stack.Declare(node->name, ObjectHolder::Own(ArrayObject()));
    return *call_stack.Find(node->name);
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::ArrayAssignment> node) {
    auto index_obj = Eval(node->index);
    auto index_ptr = index_obj.TryAs<Number>();
    if (!index_ptr) {
        Error(node.get(), "Index expr is not int");
    }
    auto* var = call_stack.Find(node->name);
    if (!var) {
        Error(node.get(), "Variable ", node->name, " undefined");
    }
    auto arr_obj = var->TryAs<ArrayObject>();
    if (!arr_obj) {
        Error(node.get(), "Not array object");
    }
    arr_obj->SetObject(index_ptr->GetValue(), Eval(node->expr));
    return *call_stack.Find(node->name);
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::Assign> node) {
    auto* obj = call_stack.Find(node->var);
    if (!obj) {
        Error(node.get(), "Variable ", node->var, " undefined");
    }

    auto val = Eval(node->expr);
    if (!val) {
        Error(node.get(), "Assign expr returned None");
    }

    *obj = val;

    return *obj;
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::FunCall> node) {
    std::vector<ObjectHolder> args;
    for (size_t i = 0; i < node->args.size(); ++i) {
        args.push_back(Eval(node->args[i]));
    }

    auto func = ir_.GetFunction(node->name);
    return Eval(func, args);
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::Function> node,
                               std::vector<ObjectHolder> args) {
    for (size_t i = 0; i < node->args.size(); ++i) {
        call_stack.PushScope();
        call_stack.Declare(node->args[i].name, args[i]);
    }
    for (auto stmt : node->body) {
        Eval(stmt);
    }
    call_stack.PopScope();
    return ObjectHolder::None();
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::IfThenElse> node) {
    auto cond = Eval(node->condition);
    auto cond_bool = cond.TryAs<Bool>();
    if (!cond_bool) {
        Error(node.get(), "If condition is not bool");
    }
    if (cond.TryAs<Bool>()->GetValue()) {
        return Eval(node->then_branch);
    } else if (node->else_branch) {
        return Eval(node->else_branch);
    }
    return ObjectHolder::None();
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::LogicOp> node) {
    auto lhs = Eval(node->lhs);
    auto rhs = Eval(node->rhs);

    if (node->op == "<") {
        return ObjectHolder::Own(ValueObject(lhs.TryAs<Number>()->GetValue() <
                                             rhs.TryAs<Number>()->GetValue()));
    } else if (node->op == ">") {
        return ObjectHolder::Own(ValueObject(lhs.TryAs<Number>()->GetValue() >
                                             rhs.TryAs<Number>()->GetValue()));
    } else {
        Error(node.get(), "Unknown arith op: ", node->op);
    }
    return ObjectHolder::None();
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::Print> node) {
    auto obj_holder = Eval(node->expr);
    if (obj_holder) obj_holder->Print(os_);
    return ObjectHolder::None();
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

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::Return> node) {
    return Eval(node->expr);
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::Var> node) {
    auto* var = call_stack.Find(node->name);
    if (!var) {
        Error(node.get(), "Variable ", node->name, " undefined");
    }
    if (!(*var)) {
        Error(node.get(), "Variable ", node->name, " uninitialized");
    }
    return *var;
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::VarDef> node) {
    call_stack.Declare(node->name, {});
    return *call_stack.Find(node->name);
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::While> node) {
    while (true) {
        auto cond = Eval(node->condition);
        auto cond_bool = cond.TryAs<Bool>();
        if (!cond_bool) {
            Error(node.get(), "If condition is not bool");
        }
        if (!cond.TryAs<Bool>()->GetValue()) {
            break;
        }
        call_stack.PushScope();
        for (auto stmt : node->body) {
            Eval(stmt);
        }
        call_stack.PopScope();
    }
    return ObjectHolder::None();
}

ObjectHolder Interpreter::Eval(std::shared_ptr<AST::ASTNode> node) {
    if (auto ar_op = std::dynamic_pointer_cast<AST::ArithOp>(node)) {
        return Eval(ar_op);

    } else if (auto arr_ac =
                   std::dynamic_pointer_cast<AST::ArrayAccess>(node)) {
        return Eval(arr_ac);
    } else if (auto arr_assign =
                   std::dynamic_pointer_cast<AST::ArrayAssignment>(node)) {
        return Eval(arr_assign);
    } else if (auto arr_decl =
                   std::dynamic_pointer_cast<AST::ArrayDeclaration>(node)) {
        return Eval(arr_decl);
    } else if (auto assign = std::dynamic_pointer_cast<AST::Assign>(node)) {
        return Eval(assign);
    } else if (auto b_lit = std::dynamic_pointer_cast<AST::BoolLiteral>(node)) {
        return ObjectHolder::Own(ValueObject(b_lit->value));
    } else if (auto ifthen = std::dynamic_pointer_cast<AST::IfThenElse>(node)) {
        return Eval(ifthen);
    } else if (auto fun_call = std::dynamic_pointer_cast<AST::FunCall>(node)) {
        return Eval(fun_call);
    } else if (auto func = std::dynamic_pointer_cast<AST::Function>(node)) {
        return Eval(func, {});
    } else if (auto int_lit = std::dynamic_pointer_cast<AST::Integer>(node)) {
        return ObjectHolder::Own(ValueObject(int_lit->value));
    } else if (auto l_op = std::dynamic_pointer_cast<AST::LogicOp>(node)) {
        return Eval(l_op);
    } else if (auto print = std::dynamic_pointer_cast<AST::Print>(node)) {
        return Eval(print);
    } else if (auto prog = std::dynamic_pointer_cast<AST::Program>(node)) {
        return Eval(prog);
    } else if (auto ret = std::dynamic_pointer_cast<AST::Return>(node)) {
        return Eval(ret);

    } else if (auto s_lit =
                   std::dynamic_pointer_cast<AST::StringLiteral>(node)) {
        return ObjectHolder::Own(ValueObject(s_lit->value));
    } else if (auto var_def = std::dynamic_pointer_cast<AST::VarDef>(node)) {
        return Eval(var_def);
    } else if (auto var = std::dynamic_pointer_cast<AST::Var>(node)) {
        return Eval(var);
    } else if (auto whil = std::dynamic_pointer_cast<AST::While>(node)) {
        return Eval(whil);
    }

    Error(node.get(), "unknown node");
    return ObjectHolder::None();
}
