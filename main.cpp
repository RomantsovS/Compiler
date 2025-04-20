#include <cassert>
#include <iostream>
#include <queue>
#include <stack>
#include <unordered_set>

#include "arithmetic_op.h"
#include "assign.h"
#include "ast.h"
#include "fun_call.h"
#include "function.h"
#include "if_then_else.h"
#include "integer.h"
#include "interpreter.h"
#include "logic_op.h"
#include "parser.hpp"
#include "print.h"
#include "scanner.h"
#include "type.h"
#include "var.h"
#include "while.h"

void visit(ASTNode* node, std::stack<std::string>& result_stack,
           std::queue<std::string>& result_queue);

int main() {
    bool use_parser = false;
    std::shared_ptr<ASTNode> ast;
    if (use_parser) {
        EzAquarii::Interpreter interpreter(ast);
        auto res = interpreter.parse();
        if (res) return res;
    } else {
        {
            auto abs_fun = std::make_shared<Function>();
            auto abs_fun2 = std::make_shared<Function>();
        }
        auto abs_fun = std::make_shared<Function>();
        abs_fun->name = "abs";
        abs_fun->args = {{"x", Type::Int}};

        {
            auto st = std::make_shared<IfThenElse>();
            auto l_op = std::make_shared<LogicOp>();
            st->condition = l_op;
            {
                l_op->op = "<";
                auto lhs = std::make_shared<Var>();
                lhs->name = "x";
                l_op->lhs = lhs;
                auto rhs = std::make_shared<Integer>();
                rhs->val = 0;
                l_op->rhs = rhs;
            }

            {
                auto st2 = std::make_shared<ArithOp>();
                st2->op = "-";
                auto lhs = std::make_shared<Integer>();
                lhs->val = 0;
                st2->lhs = lhs;
                auto rhs = std::make_shared<Var>();
                rhs->name = "x";
                st2->rhs = rhs;
                auto ret = std::make_shared<Return>();
                ret->statement = st2;
                st->then_branch = ret;
            }
            {
                auto var = std::make_shared<Var>();
                var->name = "x";
                auto ret = std::make_shared<Return>();
                ret->statement = var;
                st->else_branch = ret;
            }
            abs_fun->body.body.push_back(st);
        }
        abs_fun->return_type = Type::Int;

        auto sum_fun = std::make_shared<Function>();
        sum_fun->name = "sum";
        sum_fun->args = {{"x", Type::Int}, {"y", Type::Int}};
        sum_fun->return_type = Type::Int;

        {
            auto var = std::make_shared<VarDef>();
            var->name = "z";
            var->type = Type::Int;
            sum_fun->body.body.push_back(var);

            auto ass = std::make_shared<Assign>();
            ass->var = "z";
            auto three = std::make_shared<Integer>();
            three->val = 3;
            ass->st = three;
            sum_fun->body.body.push_back(ass);

            auto whil = std::make_shared<While>();
            {
                auto st4 = std::make_shared<LogicOp>();
                st4->op = "<";
                auto lhs = std::make_shared<Var>();
                lhs->name = "z";
                st4->lhs = lhs;
                auto rhs = std::make_shared<Integer>();
                rhs->val = 20;
                st4->rhs = rhs;
                whil->condition = st4;
            }

            auto ass2 = std::make_shared<Assign>();
            ass2->var = "z";

            auto st2 = std::make_shared<ArithOp>();
            st2->op = "+";
            auto lhs = std::make_shared<Var>();
            lhs->name = "z";
            st2->lhs = lhs;
            {
                auto st3 = std::make_shared<ArithOp>();
                st3->op = "+";
                auto lhs2 = std::make_shared<Var>();
                lhs2->name = "x";
                st3->lhs = lhs2;
                auto rhs = std::make_shared<Var>();
                rhs->name = "y";
                st3->rhs = rhs;
                st2->rhs = st3;
            }
            ass2->st = st2;

            whil->body.push_back(ass2);

            sum_fun->body.body.push_back(whil);

            auto ret = std::make_shared<Return>();
            auto v = std::make_shared<Var>();
            v->name = "z";
            ret->statement = v;
            sum_fun->body.body.push_back(ret);
        }

        auto main_fun = std::make_shared<Function>();
        main_fun->name = "main";

        auto abs_fun_call = std::make_shared<FunCall>();
        abs_fun_call->func = abs_fun;

        {
            auto st2 = std::make_shared<ArithOp>();
            st2->op = "+";
            auto lhs = std::make_shared<Integer>();
            lhs->val = -10;
            st2->lhs = lhs;

            auto sum_fun_call = std::make_shared<FunCall>();
            sum_fun_call->func = sum_fun;
            {
                auto one = std::make_shared<Integer>();
                one->val = 1;
                auto two = std::make_shared<Integer>();
                two->val = 2;
                sum_fun_call->args.push_back(one);
                sum_fun_call->args.push_back(two);
            }
            st2->rhs = sum_fun_call;
            abs_fun_call->args.push_back(st2);
        }

        auto print = std::make_shared<Print>();
        print->st = abs_fun_call;

        main_fun->body.body.push_back(print);
        main_fun->return_type = Type::Int;
        ast = main_fun;
    }
    auto* node = ast.get();
    std::stack<ASTNode*> st;

    std::unordered_set<ASTNode*> set;

    std::stack<std::string> result_stack;
    std::queue<std::string> result_queue;

    while (!st.empty() || node) {
        if (node) {
            st.push(node);
            if (auto f = dynamic_cast<Function*>(node); f) {
                for (auto& s : f->body.body) {
                    if (!set.count(s.get())) {
                        node = s.get();
                        break;
                    }
                }
            } else if (auto pr = dynamic_cast<Print*>(node); pr) {
                node = pr->st.get();
            } else if (auto fc = dynamic_cast<FunCall*>(node); fc) {
                node = fc->func.get();
            } else if (auto ifthen = dynamic_cast<IfThenElse*>(node); ifthen) {
                if (!set.count(ifthen->condition.get()))
                    node = ifthen->condition.get();
                else if (!set.count(ifthen->then_branch.get()))
                    node = ifthen->then_branch.get();
                else if (!set.count(ifthen->else_branch.get()))
                    node = ifthen->else_branch.get();
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
            } else if (auto var_def = dynamic_cast<VarDef*>(node); var_def) {
                node = nullptr;
            } else if (auto var = dynamic_cast<Var*>(node); var) {
                node = nullptr;
            } else if (auto integer = dynamic_cast<Integer*>(node); integer) {
                node = nullptr;
            } else if (auto ass = dynamic_cast<Assign*>(node); ass) {
                if (!set.count(ass->st.get()))
                    node = ass->st.get();
                else
                    node = nullptr;
            } else if (auto whil = dynamic_cast<While*>(node); whil) {
                for (auto& s : whil->body) {
                    if (!set.count(s.get())) {
                        node = s.get();
                        break;
                    }
                }
                if (node == whil) {
                    if (!set.count(whil->condition.get())) {
                        node = whil->condition.get();
                    } else {
                        node = nullptr;
                    }
                }
            } else {
                throw std::logic_error("err");
            }
        } else {
            auto* cur = st.top();
            st.pop();
            if (auto f = dynamic_cast<Function*>(cur); f) {
                for (auto& s : f->body.body) {
                    if (!set.count(s.get())) {
                        st.push(cur);
                        node = s.get();
                        break;
                    }
                }
                if (!node) {
                    visit(cur, result_stack, result_queue);
                    set.insert(cur);
                }
            } else if (auto pr = dynamic_cast<Print*>(cur); pr) {
                visit(cur, result_stack, result_queue);
                set.insert(cur);
            } else if (auto fc = dynamic_cast<FunCall*>(cur); fc) {
                if (!set.count(fc->func.get())) {
                    st.push(cur);
                    node = fc->func.get();
                } else {
                    for (auto& s : fc->args) {
                        if (!set.count(s.get())) {
                            st.push(cur);
                            node = s.get();
                            break;
                        }
                    }
                    if (!node) {
                        visit(cur, result_stack, result_queue);
                        set.insert(cur);
                    }
                }
            } else if (auto ifthen = dynamic_cast<IfThenElse*>(cur); ifthen) {
                if (!set.count(ifthen->then_branch.get())) {
                    st.push(cur);
                    node = ifthen->then_branch.get();
                } else if (!set.count(ifthen->else_branch.get())) {
                    st.push(cur);
                    node = ifthen->else_branch.get();
                } else {
                    visit(cur, result_stack, result_queue);
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
                    visit(cur, result_stack, result_queue);
                    set.insert(cur);
                }
            } else if (auto ret = dynamic_cast<Return*>(cur); ret) {
                visit(cur, result_stack, result_queue);
                set.insert(cur);
            } else if (auto arith_op = dynamic_cast<ArithOp*>(cur); arith_op) {
                if (!set.count(arith_op->lhs.get())) {
                    st.push(cur);
                    node = arith_op->lhs.get();
                } else if (!set.count(arith_op->rhs.get())) {
                    st.push(cur);
                    node = arith_op->rhs.get();
                } else {
                    visit(cur, result_stack, result_queue);
                    set.insert(cur);
                }
            } else if (auto var_def = dynamic_cast<VarDef*>(cur); var_def) {
                visit(cur, result_stack, result_queue);
                set.insert(cur);
            } else if (auto var = dynamic_cast<Var*>(cur); var) {
                visit(cur, result_stack, result_queue);
                set.insert(cur);
            } else if (auto integer = dynamic_cast<Integer*>(cur); integer) {
                visit(cur, result_stack, result_queue);
                set.insert(cur);
            } else if (auto ass = dynamic_cast<Assign*>(cur); ass) {
                visit(cur, result_stack, result_queue);
                set.insert(cur);
            } else if (auto whil = dynamic_cast<While*>(cur); whil) {
                for (auto& s : whil->body) {
                    if (!set.count(s.get())) {
                        node = s.get();
                        break;
                    }
                }
                if (!node) {
                    if (!set.count(whil->condition.get())) {
                        node = whil->condition.get();
                    } else {
                        visit(cur, result_stack, result_queue);
                        set.insert(cur);
                    }
                }
            } else {
                throw std::logic_error("err");
            }
        }
    }

    assert(result_stack.empty());

    std::cout << "#include <iostream>\n\n";

    while (!result_queue.empty()) {
        std::cout << result_queue.front() << "\n\n";
        result_queue.pop();
    }

    return 0;
}

void visit(ASTNode* node, std::stack<std::string>& result_stack,
           std::queue<std::string>& result_queue) {
    if (auto f = dynamic_cast<Function*>(node); f) {
        auto str = to_string(f->return_type) + " " + f->name + "(";
        for (size_t i = 0; i < f->args.size(); ++i) {
            if (i > 0) str += ", ";
            str += to_string(f->args[i].type) + " " + f->args[i].name;
        }
        str += ") {\n";
        assert(result_stack.size() >= f->body.body.size());
        std::stack<std::string> temp_st;
        for (size_t i = 0; i < f->body.body.size(); ++i) {
            temp_st.push(result_stack.top());
            result_stack.pop();
        }
        for (size_t i = 0; i < f->body.body.size(); ++i) {
            if (i > 0) str += "\n";
            str += temp_st.top();
            temp_st.pop();
        }
        str += "\n}";
        result_queue.push(str);
    } else if (auto pr = dynamic_cast<Print*>(node); pr) {
        assert(!result_stack.empty());
        auto a = result_stack.top();
        result_stack.pop();
        result_stack.push("std::cout << (" + a + ");");
    } else if (auto fc = dynamic_cast<FunCall*>(node); fc) {
        assert(result_stack.size() >= fc->args.size());
        std::stack<std::string> temp_st;
        for (size_t i = 0; i < fc->args.size(); ++i) {
            temp_st.push(result_stack.top());
            result_stack.pop();
        }
        auto str = fc->func->name + "(";
        for (size_t i = 0; i < fc->args.size(); ++i) {
            if (i > 0) str += ", ";
            str += temp_st.top();
            temp_st.pop();
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
        result_stack.push("(" + b + ") " + arith_op->op + " (" + a + ")");
    } else if (auto var_def = dynamic_cast<VarDef*>(node); var_def) {
        result_stack.push(to_string(var_def->type) + " " + var_def->name + ";");
    } else if (auto var = dynamic_cast<Var*>(node); var) {
        result_stack.push(var->name);
    } else if (auto integer = dynamic_cast<Integer*>(node); integer) {
        result_stack.push(std::to_string(integer->val));
    } else if (auto ass = dynamic_cast<Assign*>(node); ass) {
        assert(result_stack.size() >= 1);
        auto a = result_stack.top();
        result_stack.pop();
        result_stack.push(ass->var + " = " + a + ";");
    } else if (auto whil = dynamic_cast<While*>(node); whil) {
        assert(result_stack.size() >= whil->body.size() + 1);
        auto cond = result_stack.top();
        result_stack.pop();
        std::stack<std::string> temp_st;
        for (size_t i = 0; i < whil->body.size(); ++i) {
            temp_st.push(result_stack.top());
            result_stack.pop();
        }
        auto str = "while (" + cond + ") {\n";
        for (size_t i = 0; i < whil->body.size(); ++i) {
            if (i > 0) str += "\n";
            str += temp_st.top();
            temp_st.pop();
        }
        str += "\n}\n";
        result_stack.push(str);
    } else {
        throw std::logic_error("err");
    }
}
