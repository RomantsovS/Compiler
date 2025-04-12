#include <iostream>
#include <stack>
#include <unordered_set>

#include "arithmetic_op.h"
#include "fun_call.h"
#include "function.h"
#include "if_then_else.h"
#include "integer.h"
#include "logic_op.h"
#include "print.h"
#include "var.h"

void visit(Statement* node, std::stack<std::string>& result_stack);

int main() {
    auto abs_fun = std::make_unique<Function>();
    abs_fun->name = "abs";

    abs_fun->args = {{"x", "int"}};

    {
        auto st = std::make_unique<IfThenElse>();
        st->op.op = "<";
        st->op.var.name = "x";
        st->op.value = Integer{0};

        {
            auto st2 = std::make_unique<ArithOp>();
            st2->op = "-";
            st2->value = Integer{0};
            st2->var.name = "x";
            st->return_true.statement = std::move(st2);
        }
        {
            auto st2 = std::make_unique<Var>();
            st2->name = "x";
            st->return_false.statement = std::move(st2);
        }
        abs_fun->body.push_back(std::move(st));
    }
    abs_fun->return_type = "int";

    Function main_fun;
    main_fun.name = "main";

    auto fun_call = std::make_unique<FunCall>();
    fun_call->func = abs_fun.get();
    fun_call->args = {Integer{10}};
    auto print = std::make_unique<Print>();
    print->st = std::move(fun_call);

    main_fun.fun.push_back(std::move(abs_fun));

    main_fun.body.push_back(std::move(print));
    main_fun.return_type = "int";

    std::stack<Statement*> st;
    Statement* last = nullptr;
    Statement* node = &main_fun;
    std::unordered_set<Statement*> set;

    std::stack<std::string> result_stack;

    while (!st.empty() || node) {
        if (node) {
            st.push(node);
            if (auto f = dynamic_cast<Function*>(node); f) {
                for (auto& s : f->body) {
                    if (!set.count(s.get())) {
                        node = s.get();
                        break;
                    }
                }
            } else if (auto pr = dynamic_cast<Print*>(node); pr) {
                if (!set.count(pr->st.get())) node = pr->st.get();
            } else if (auto fc = dynamic_cast<FunCall*>(node); fc) {
                node = fc->func;
            } else if (auto ifthen = dynamic_cast<IfThenElse*>(node); ifthen) {
                if (!set.count(&ifthen->op))
                    node = &ifthen->op;
                else if (!set.count(&ifthen->return_true))
                    node = &ifthen->return_true;
                else if (!set.count(&ifthen->return_false))
                    node = &ifthen->return_false;
                else
                    node = nullptr;
            } else if (auto logic_op = dynamic_cast<LogicOp*>(node); logic_op) {
                node = nullptr;
            } else if (auto ret = dynamic_cast<Return*>(node); ret) {
                node = ret->statement.get();
            } else if (auto arith_op = dynamic_cast<ArithOp*>(node); arith_op) {
                node = nullptr;
            } else if (auto var = dynamic_cast<Var*>(node); var) {
                node = nullptr;
            } else {
                throw std::logic_error("err");
            }
        } else {
            auto* cur = st.top();
            st.pop();
            if (auto f = dynamic_cast<Function*>(cur); f) {
                for (auto& s : f->body) {
                    if (!set.count(s.get()) && last != s.get()) {
                        node = s.get();
                        break;
                    }
                }
                if (!node) {
                    visit(cur, result_stack);
                    set.insert(cur);
                }
            } else if (auto pr = dynamic_cast<Print*>(cur); pr) {
                visit(cur, result_stack);
                set.insert(cur);
            } else if (auto fc = dynamic_cast<FunCall*>(cur); fc) {
                visit(cur, result_stack);
                set.insert(cur);
            } else if (auto ifthen = dynamic_cast<IfThenElse*>(cur); ifthen) {
                if (!set.count(&ifthen->return_true)) {
                    st.push(cur);
                    node = &ifthen->return_true;
                } else if (!set.count(&ifthen->return_false)) {
                    st.push(cur);
                    node = &ifthen->return_false;
                } else {
                    visit(cur, result_stack);
                    set.insert(cur);
                }
            } else if (auto logic_op = dynamic_cast<LogicOp*>(cur); logic_op) {
                visit(cur, result_stack);
                set.insert(cur);
            } else if (auto ret = dynamic_cast<Return*>(cur); ret) {
                visit(cur, result_stack);
                set.insert(cur);
            } else if (auto arith_op = dynamic_cast<ArithOp*>(cur); arith_op) {
                visit(cur, result_stack);
                set.insert(cur);
            } else if (auto var = dynamic_cast<Var*>(cur); var) {
                visit(cur, result_stack);
                set.insert(cur);
            } else {
                throw std::logic_error("err");
            }
        }
    }

    auto main_str = result_stack.top();
    result_stack.pop();

    std::cout << "#include <iostream>\n\n";

    while (!result_stack.empty()) {
        std::cout << result_stack.top() << "\n\n";
        result_stack.pop();
    }

    std::cout << main_str;

    return 0;
}

void visit(Statement* node, std::stack<std::string>& result_stack) {
    if (auto f = dynamic_cast<Function*>(node); f) {
        auto a = result_stack.top();
        result_stack.pop();
        auto str = f->return_type + " " + f->name + "(";
        for (int i = 0; i < f->args.size(); ++i) {
            if (i > 0) str += ", ";
            str += f->args[i].type + " " + f->args[i].name;
        }
        str += ") {\n" + a + "\n}";
        result_stack.push(str);
    } else if (auto pr = dynamic_cast<Print*>(node); pr) {
        auto a = result_stack.top();
        result_stack.pop();
        result_stack.push("std::cout << " + a + ";");
    } else if (auto fc = dynamic_cast<FunCall*>(node); fc) {
        auto str = fc->func->name + "(";
        for (int i = 0; i < fc->args.size(); ++i) {
            if (i > 0) str += ", ";
            str += std::to_string(fc->args[i].val);
        }
        str += ")";
        result_stack.push(str);
    } else if (auto ifthen = dynamic_cast<IfThenElse*>(node); ifthen) {
        auto a = result_stack.top();
        result_stack.pop();
        auto b = result_stack.top();
        result_stack.pop();
        auto c = result_stack.top();
        result_stack.pop();
        result_stack.push(std::string("if (") + c + ") { " + b + " } else { " +
                          a + " }");
    } else if (auto logic_op = dynamic_cast<LogicOp*>(node); logic_op) {
        result_stack.push(logic_op->var.name + " " + logic_op->op + " " +
                          std::to_string(logic_op->value.val));
    } else if (auto ret = dynamic_cast<Return*>(node); ret) {
        auto a = result_stack.top();
        result_stack.pop();
        result_stack.push("return " + a + ";");
    } else if (auto arith_op = dynamic_cast<ArithOp*>(node); arith_op) {
        result_stack.push(arith_op->var.name + " " + arith_op->op + " " +
                          std::to_string(arith_op->value.val));
    } else if (auto var = dynamic_cast<Var*>(node); var) {
        result_stack.push(var->name);
    } else {
        throw std::logic_error("err");
    }
}
