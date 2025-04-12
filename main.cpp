#include <cassert>
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

    abs_fun->args = {{"x", Type::INT}};

    {
        auto st = std::make_unique<IfThenElse>();
        st->op.op = "<";
        {
            auto lhs = std::make_unique<Var>();
            lhs->name = "x";
            st->op.lhs = std::move(lhs);
            auto rhs = std::make_unique<Integer>();
            rhs->val = 0;
            st->op.rhs = std::move(rhs);
        }

        {
            auto st2 = std::make_unique<ArithOp>();
            st2->op = "-";
            auto lhs = std::make_unique<Integer>();
            lhs->val = 0;
            st2->lhs = std::move(lhs);
            auto rhs = std::make_unique<Var>();
            rhs->name = "x";
            st2->rhs = std::move(rhs);
            st->return_true.statement = std::move(st2);
        }
        {
            auto var = std::make_unique<Var>();
            var->name = "x";
            st->return_false.statement = std::move(var);
        }
        abs_fun->body.push_back(std::move(st));
    }
    abs_fun->return_type = Type::INT;

    Function main_fun;
    main_fun.name = "main";

    auto fun_call = std::make_unique<FunCall>();
    fun_call->func = abs_fun.get();

    {
        auto st2 = std::make_unique<ArithOp>();
        st2->op = "+";
        auto lhs = std::make_unique<Integer>();
        lhs->val = -10;
        st2->lhs = std::move(lhs);
        auto rhs = std::make_unique<Integer>();
        rhs->val = 8;
        st2->rhs = std::move(rhs);
        fun_call->args.push_back(std::move(st2));
    }

    auto print = std::make_unique<Print>();
    print->st = std::move(fun_call);

    main_fun.fun.push_back(std::move(abs_fun));

    main_fun.body.push_back(std::move(print));
    main_fun.return_type = Type::VOID;

    std::stack<Statement*> st;
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
                node = pr->st.get();
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
                if (!set.count(logic_op->lhs.get()))
                    node = logic_op->lhs.get();
                else if (!set.count(logic_op->rhs.get()))
                    node = logic_op->rhs.get();
                else
                    node = nullptr;
            } else if (auto ret = dynamic_cast<Return*>(node); ret) {
                node = ret->statement.get();
            } else if (auto arith_op = dynamic_cast<ArithOp*>(node); arith_op) {
                if (!set.count(arith_op->lhs.get()))
                    node = arith_op->lhs.get();
                else if (!set.count(arith_op->rhs.get()))
                    node = arith_op->rhs.get();
                else
                    node = nullptr;
            } else if (auto var = dynamic_cast<Var*>(node); var) {
                node = nullptr;
            } else if (auto integer = dynamic_cast<Integer*>(node); integer) {
                node = nullptr;
            } else {
                throw std::logic_error("err");
            }
        } else {
            auto* cur = st.top();
            st.pop();
            if (auto f = dynamic_cast<Function*>(cur); f) {
                for (auto& s : f->body) {
                    if (!set.count(s.get())) {
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
                if (!set.count(fc->func)) {
                    st.push(cur);
                    node = fc->func;
                } else {
                    for (auto& s : fc->args) {
                        if (!set.count(s.get())) {
                            st.push(cur);
                            node = s.get();
                            break;
                        }
                    }
                    if (!node) {
                        visit(cur, result_stack);
                        set.insert(cur);
                    }
                }
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
                if (!set.count(logic_op->lhs.get())) {
                    st.push(cur);
                    node = logic_op->lhs.get();
                } else if (!set.count(logic_op->rhs.get())) {
                    st.push(cur);
                    node = logic_op->rhs.get();
                } else {
                    visit(cur, result_stack);
                    set.insert(cur);
                }
            } else if (auto ret = dynamic_cast<Return*>(cur); ret) {
                visit(cur, result_stack);
                set.insert(cur);
            } else if (auto arith_op = dynamic_cast<ArithOp*>(cur); arith_op) {
                if (!set.count(arith_op->lhs.get())) {
                    st.push(cur);
                    node = arith_op->lhs.get();
                } else if (!set.count(arith_op->rhs.get())) {
                    st.push(cur);
                    node = arith_op->rhs.get();
                } else {
                    visit(cur, result_stack);
                    set.insert(cur);
                }
            } else if (auto var = dynamic_cast<Var*>(cur); var) {
                visit(cur, result_stack);
                set.insert(cur);
            } else if (auto integer = dynamic_cast<Integer*>(cur); integer) {
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
        assert(!result_stack.empty());
        auto a = result_stack.top();
        result_stack.pop();
        auto str = to_string(f->return_type) + " " + f->name + "(";
        for (int i = 0; i < f->args.size(); ++i) {
            if (i > 0) str += ", ";
            str += to_string(f->args[i].type) + " " + f->args[i].name;
        }
        str += ") {\n" + a + "\n}";
        result_stack.push(str);
    } else if (auto pr = dynamic_cast<Print*>(node); pr) {
        assert(!result_stack.empty());
        auto a = result_stack.top();
        result_stack.pop();
        result_stack.push("std::cout << (" + a + ");");
    } else if (auto fc = dynamic_cast<FunCall*>(node); fc) {
        auto str = fc->func->name + "(";
        for (int i = 0; i < fc->args.size(); ++i) {
            if (i > 0) str += ", ";
            assert(!result_stack.empty());
            str += result_stack.top();
            result_stack.pop();
        }
        str += ")";
        result_stack.push(str);
    } else if (auto ifthen = dynamic_cast<IfThenElse*>(node); ifthen) {
        assert(result_stack.size() >= 3);
        auto a = result_stack.top();
        result_stack.pop();
        auto b = result_stack.top();
        result_stack.pop();
        auto c = result_stack.top();
        result_stack.pop();
        result_stack.push(std::string("if (") + c + ") { " + b + " } else { " +
                          a + " }");
    } else if (auto logic_op = dynamic_cast<LogicOp*>(node); logic_op) {
        assert(result_stack.size() >= 2);
        auto a = result_stack.top();
        result_stack.pop();
        auto b = result_stack.top();
        result_stack.pop();
        result_stack.push(b + " " + logic_op->op + " " + a);
    } else if (auto ret = dynamic_cast<Return*>(node); ret) {
        assert(!result_stack.empty());
        auto a = result_stack.top();
        result_stack.pop();
        result_stack.push("return " + a + ";");
    } else if (auto arith_op = dynamic_cast<ArithOp*>(node); arith_op) {
        assert(result_stack.size() >= 2);
        auto a = result_stack.top();
        result_stack.pop();
        auto b = result_stack.top();
        result_stack.pop();
        result_stack.push(b + " " + arith_op->op + " " + a);
    } else if (auto var = dynamic_cast<Var*>(node); var) {
        result_stack.push(var->name);
    } else if (auto integer = dynamic_cast<Integer*>(node); integer) {
        result_stack.push(std::to_string(integer->val));
    } else {
        throw std::logic_error("err");
    }
}
