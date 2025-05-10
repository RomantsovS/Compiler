#include "interpreter.h"

#include <algorithm>
#include <random>
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
#include "ir.h"

int Interpreter::Exec() {
    auto obj = Eval(static_cast<AST::Program*>(ir_.GetAST().get()));

    auto number_ptr = obj.TryAs<Number>();
    if (number_ptr) return number_ptr->GetValue();

    return 1;
}

ObjectHolder Interpreter::Eval(AST::ArithOp* node) {
    auto lhs = Eval(node->lhs.get());
    auto rhs = Eval(node->rhs.get());

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
        auto r_val = rhs.TryAs<Number>()->GetValue();
        if (r_val == 0) {
            Error(node, "division by zero");
        }
        return ObjectHolder::Own(
            ValueObject(lhs.TryAs<Number>()->GetValue() / r_val));
    } else if (node->op == "%") {
        return ObjectHolder::Own(ValueObject(lhs.TryAs<Number>()->GetValue() %
                                             rhs.TryAs<Number>()->GetValue()));
    } else {
        Error(node, "Unknown arith op: ", node->op);
    }
    return ObjectHolder::None();
}

ObjectHolder Interpreter::Eval(AST::ArrayAccess* node) {
    auto index_obj = Eval(node->index.get());
    auto index_ptr = index_obj.TryAs<Number>();
    if (!index_ptr) {
        Error(node, "Index expr is not int");
    }
    auto* var = call_stack.Find(node->name);
    if (!var) {
        Error(node, "Variable ", node->name, " undefined");
    }
    auto arr_obj = var->TryAs<ArrayObject>();
    if (!arr_obj) {
        Error(node, "Not array object");
    }
    try {
        return arr_obj->GetObject(index_ptr->GetValue());
    } catch (std::exception& ex) {
        Error(node, ex.what());
    }
    return ObjectHolder::None();
}

ObjectHolder Interpreter::Eval(AST::ArrayDeclaration* node) {
    call_stack.Declare(node->name, ObjectHolder::Own(ArrayObject()));
    return *call_stack.Find(node->name);
}

ObjectHolder Interpreter::Eval(AST::ArrayAssignment* node) {
    auto index_obj = Eval(node->index.get());
    auto index_ptr = index_obj.TryAs<Number>();
    if (!index_ptr) {
        Error(node, "Index expr is not int");
    }
    auto* var = call_stack.Find(node->name);
    if (!var) {
        Error(node, "Variable ", node->name, " undefined");
    }
    auto arr_obj = var->TryAs<ArrayObject>();
    if (!arr_obj) {
        Error(node, "Not array object");
    }
    arr_obj->SetObject(index_ptr->GetValue(), Eval(node->expr.get()));
    return *call_stack.Find(node->name);
}

ObjectHolder Interpreter::Eval(AST::Assign* node) {
    auto* obj = call_stack.Find(node->var);
    if (!obj) {
        Error(node, "Variable ", node->var, " undefined");
    }

    auto val = Eval(node->expr.get());
    if (!val) {
        Error(node, "Assign expr returned None");
    }

    *obj = val;

    return *obj;
}

ObjectHolder Interpreter::Eval(AST::BoolLiteral* node) {
    return ObjectHolder::Own(ValueObject(node->value));
}

ObjectHolder Interpreter::Eval(AST::FunCall* node) {
    std::vector<ObjectHolder> args;
    for (size_t i = 0; i < node->args.size(); ++i) {
        args.push_back(Eval(node->args[i].get()));
    }

    return Eval(ir_.GetFunction(node->name).get(), args);
}

ObjectHolder Interpreter::Eval(AST::Function* node,
                               std::vector<ObjectHolder> args) {
    call_stack.PushScope();
    for (size_t i = 0; i < node->args.size(); ++i) {
        call_stack.Declare(node->args[i].name, args[i]);
    }
    ObjectHolder return_copy;
    for (auto stmt : node->body) {
        Eval(stmt.get());
        if (return_result) {
            return_copy = *return_result;
            return_result.release();
            break;
        }
    }
    call_stack.PopScope();
    return return_copy;
}

ObjectHolder Interpreter::Eval(AST::IfThenElse* node) {
    auto cond = Eval(node->condition.get());
    auto cond_bool = cond.TryAs<Bool>();
    if (!cond_bool) {
        Error(node, "If condition is not bool");
    }
    if (cond.TryAs<Bool>()->GetValue()) {
        call_stack.PushScope();
        for (auto stmt : node->then_branch) {
            Eval(stmt.get());
            if (return_result) {
                break;
            }
        }
        call_stack.PopScope();
        return ObjectHolder::None();
    } else if (!node->else_branch.empty()) {
        call_stack.PushScope();
        for (auto stmt : node->else_branch) {
            Eval(stmt.get());
            if (return_result) {
                break;
            }
        }
        call_stack.PopScope();
        return ObjectHolder::None();
    }
    return ObjectHolder::None();
}

ObjectHolder Interpreter::Eval(AST::Integer* node) {
    return ObjectHolder::Own(ValueObject(node->value));
}

ObjectHolder Interpreter::Eval(AST::LogicOp* node) {
    auto lhs = Eval(node->lhs.get());
    auto rhs = Eval(node->rhs.get());

    if (node->op == "<") {
        return ObjectHolder::Own(ValueObject(lhs.TryAs<Number>()->GetValue() <
                                             rhs.TryAs<Number>()->GetValue()));
    } else if (node->op == ">") {
        return ObjectHolder::Own(ValueObject(lhs.TryAs<Number>()->GetValue() >
                                             rhs.TryAs<Number>()->GetValue()));
    } else if (node->op == "==") {
        return ObjectHolder::Own(ValueObject(lhs.TryAs<Number>()->GetValue() ==
                                             rhs.TryAs<Number>()->GetValue()));
    } else {
        Error(node, "Unknown logic op: ", node->op);
    }
    return ObjectHolder::None();
}

ObjectHolder Interpreter::Eval(AST::Print* node) {
    auto obj_holder = Eval(node->expr.get());
    if (obj_holder) obj_holder->Print(os_);
    return ObjectHolder::None();
}

ObjectHolder Interpreter::Eval(AST::Program* node) {
    AST::Function* main_func = nullptr;

    for (auto global : node->globals) {
        auto func = std::dynamic_pointer_cast<AST::Function>(global);
        if (func) {
            if (func->name == "main") main_func = func.get();
        } else {
            Eval(global.get());
        }
    }

    if (!main_func) {
        Error(node, "main function undefined");
    }

    return Eval(main_func);
}

ObjectHolder Interpreter::Eval(AST::Rand* node) {
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, RAND_MAX);

    return ObjectHolder::Own(ValueObject(dist(rng)));
}

ObjectHolder Interpreter::Eval(AST::Return* node) {
    return_result = std::make_unique<ObjectHolder>(
        node->expr ? Eval(node->expr.get()) : ObjectHolder::None());
    return *return_result;
}

ObjectHolder Interpreter::Eval(AST::StringLiteral* node) {
    return ObjectHolder::Own(ValueObject(node->value));
}

ObjectHolder Interpreter::Eval(AST::Var* node) {
    auto* var = call_stack.Find(node->name);
    if (!var) {
        Error(node, "Variable ", node->name, " undefined");
    }
    if (!(*var)) {
        Error(node, "Variable ", node->name, " uninitialized");
    }
    return *var;
}

ObjectHolder Interpreter::Eval(AST::VarDef* node) {
    call_stack.Declare(node->name, {});
    return *call_stack.Find(node->name);
}

ObjectHolder Interpreter::Eval(AST::While* node) {
    bool stop = false;
    while (!stop) {
        auto cond = Eval(node->condition.get());
        auto cond_bool = cond.TryAs<Bool>();
        if (!cond_bool) {
            Error(node, "If condition is not bool");
        }
        if (!cond.TryAs<Bool>()->GetValue()) {
            break;
        }
        call_stack.PushScope();
        for (auto stmt : node->body) {
            Eval(stmt.get());
            if (return_result) {
                stop = true;
            }
        }
        call_stack.PopScope();
    }
    return ObjectHolder::None();
}

ObjectHolder Interpreter::Eval(AST::ASTNode* node) {
    switch (node->node_type) {
        case AST::NodeType::ArithOp: {
            return Eval(static_cast<AST::ArithOp*>(node));
        }
        case AST::NodeType::ArrayAccess: {
            return Eval(static_cast<AST::ArrayAccess*>(node));
        }
        case AST::NodeType::ArrayAssignment: {
            return Eval(static_cast<AST::ArrayAssignment*>(node));
        }
        case AST::NodeType::ArrayDeclaration: {
            return Eval(static_cast<AST::ArrayDeclaration*>(node));
        }
        case AST::NodeType::Assign: {
            return Eval(static_cast<AST::Assign*>(node));
        }
        case AST::NodeType::BoolLiteral: {
            return Eval(static_cast<AST::BoolLiteral*>(node));
        }
        case AST::NodeType::FunCall: {
            return Eval(static_cast<AST::FunCall*>(node));
        }
        case AST::NodeType::Function: {
            return Eval(static_cast<AST::Function*>(node), {});
        }
        case AST::NodeType::IfThenElse: {
            return Eval(static_cast<AST::IfThenElse*>(node));
        }
        case AST::NodeType::Integer: {
            return Eval(static_cast<AST::Integer*>(node));
        }
        case AST::NodeType::LogicOp: {
            return Eval(static_cast<AST::LogicOp*>(node));
        }
        case AST::NodeType::Print: {
            return Eval(static_cast<AST::Print*>(node));
        }
        case AST::NodeType::Program: {
            return Eval(static_cast<AST::Program*>(node));
        }
        case AST::NodeType::Rand: {
            return Eval(static_cast<AST::Rand*>(node));
        }
        case AST::NodeType::Return: {
            return Eval(static_cast<AST::Return*>(node));
        }
        case AST::NodeType::StringLiteral: {
            return Eval(static_cast<AST::StringLiteral*>(node));
        }
        case AST::NodeType::Var: {
            return Eval(static_cast<AST::Var*>(node));
        }
        case AST::NodeType::VarDef: {
            return Eval(static_cast<AST::VarDef*>(node));
        }
        case AST::NodeType::While: {
            return Eval(static_cast<AST::While*>(node));
        }
        default:
            Error(node, "unknown node");
    }

    return ObjectHolder::None();
}
