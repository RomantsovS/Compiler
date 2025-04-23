#include <cassert>
#include <deque>
#include <iostream>
#include <stack>
#include <typeinfo>
#include <unordered_set>

#include "ast/arithmetic_op.h"
#include "ast/array.h"
#include "ast/assign.h"
#include "ast/ast.h"
#include "ast/bool_literal.h"
#include "ast/fun_call.h"
#include "ast/function.h"
#include "ast/if_then_else.h"
#include "ast/integer.h"
#include "ast/logic_op.h"
#include "ast/print.h"
#include "ast/program.h"
#include "ast/string_literal.h"
#include "ast/type.h"
#include "ast/var.h"
#include "ast/while.h"
#include "interpreter.h"
#include "parser.hpp"
#include "scanner.h"

void visit(ASTNode* node, std::stack<std::string>& result_stack,
           std::deque<std::string>& result_deque,
           std::unordered_set<ASTNode*>& visited);

int main() {
    bool use_parser = true;
    std::shared_ptr<ASTNode> ast;
    if (use_parser) {
        EzAquarii::Interpreter interpreter(ast);
        auto res = interpreter.parse();
        if (res) return res;
    } else {
        auto prog = std::make_shared<Program>();

        auto abs_fun = std::make_shared<Function>();
        prog->functions.push_back(abs_fun);

        abs_fun->name = "abs";
        abs_fun->args = {{"x", Type::Int()}};

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
            abs_fun->body.push_back(st);
        }
        abs_fun->return_type = Type::Int();

        auto sum_fun = std::make_shared<Function>();
        prog->functions.push_back(sum_fun);

        sum_fun->name = "sum";
        sum_fun->args = {{"x", Type::Int()}, {"y", Type::Int()}};
        sum_fun->return_type = Type::Int();

        {
            auto var = std::make_shared<VarDef>();
            var->name = "z";
            var->type = Type::Int();
            sum_fun->body.push_back(var);

            auto ass = std::make_shared<Assign>();
            ass->var = "z";
            auto three = std::make_shared<Integer>();
            three->val = 3;
            ass->st = three;
            sum_fun->body.push_back(ass);

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

            sum_fun->body.push_back(whil);

            auto ret = std::make_shared<Return>();
            auto v = std::make_shared<Var>();
            v->name = "z";
            ret->statement = v;
            sum_fun->body.push_back(ret);
        }

        auto main_fun = std::make_shared<Function>();
        prog->functions.push_back(main_fun);

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

        main_fun->body.push_back(print);
        main_fun->return_type = Type::Int();
        ast = prog;
    }
    auto* node = ast.get();
    std::stack<ASTNode*> st;

    std::unordered_set<ASTNode*> visited;
    std::stack<std::string> result_stack;
    std::deque<std::string> result_deque;

    while (!st.empty() || node) {
        if (node) {
            assert(st.empty() || st.top() != node);
            st.push(node);
            auto* cur = node;
            node = nullptr;
            if (auto pr = dynamic_cast<Program*>(cur); pr) {
                for (auto& gl : pr->globals) {
                    if (!visited.count(gl.get())) {
                        node = gl.get();
                        break;
                    }
                }
                if (node) continue;
                for (auto& f : pr->functions) {
                    if (!visited.count(f.get())) {
                        node = f.get();
                        break;
                    }
                }
            } else if (auto f = dynamic_cast<Function*>(cur); f) {
                for (auto& s : f->body) {
                    if (!visited.count(s.get())) {
                        node = s.get();
                        break;
                    }
                }
            } else if (auto pr = dynamic_cast<Print*>(cur); pr) {
                node = pr->st.get();
            } else if (auto fc = dynamic_cast<FunCall*>(cur); fc) {
                node = fc->func.get();
            } else if (auto ifthen = dynamic_cast<IfThenElse*>(cur); ifthen) {
                if (!visited.count(ifthen->condition.get()))
                    node = ifthen->condition.get();
                else if (!visited.count(ifthen->then_branch.get()))
                    node = ifthen->then_branch.get();
                else if (!visited.count(ifthen->else_branch.get()))
                    node = ifthen->else_branch.get();
            } else if (auto logic_op = dynamic_cast<LogicOp*>(cur); logic_op) {
                if (!visited.count(logic_op->lhs.get()))
                    node = logic_op->lhs.get();
                else if (!visited.count(logic_op->rhs.get()))
                    node = logic_op->rhs.get();
            } else if (auto ret = dynamic_cast<Return*>(cur); ret) {
                node = ret->statement.get();
            } else if (auto arith_op = dynamic_cast<ArithOp*>(cur); arith_op) {
                if (!visited.count(arith_op->lhs.get()))
                    node = arith_op->lhs.get();
                else if (!visited.count(arith_op->rhs.get()))
                    node = arith_op->rhs.get();
            } else if (auto var_def = dynamic_cast<VarDef*>(cur); var_def) {
                ;
            } else if (auto var = dynamic_cast<Var*>(cur); var) {
                ;
            } else if (auto integer = dynamic_cast<Integer*>(cur); integer) {
                ;
            } else if (auto ass = dynamic_cast<Assign*>(cur); ass) {
                if (!visited.count(ass->st.get())) node = ass->st.get();
            } else if (auto whil = dynamic_cast<While*>(cur); whil) {
                for (auto& s : whil->body) {
                    if (!visited.count(s.get())) {
                        node = s.get();
                        break;
                    }
                }
                if (!node) {
                    if (!visited.count(whil->condition.get())) {
                        node = whil->condition.get();
                    }
                }
            } else if (auto str = dynamic_cast<StringLiteral*>(cur); str) {
                ;
            } else if (auto str = dynamic_cast<BoolLiteral*>(cur); str) {
                ;
            } else if (auto arr = dynamic_cast<ArrayDeclaration*>(cur); arr) {
                ;
            } else if (auto arr_ac = dynamic_cast<ArrayAccess*>(cur); arr_ac) {
                if (!visited.count(arr_ac->index.get())) {
                    node = arr_ac->index.get();
                }
            } else if (auto arr_as = dynamic_cast<ArrayAssignment*>(cur);
                       arr_as) {
                if (!visited.count(arr_as->index.get())) {
                    node = arr_as->index.get();
                } else if (!visited.count(arr_as->value.get())) {
                    node = arr_as->value.get();
                }
            } else {
                throw std::logic_error("err"s + typeid(cur).name());
            }
        } else {
            auto* cur = st.top();
            st.pop();
            if (auto pr = dynamic_cast<Program*>(cur); pr) {
                for (auto& gl : pr->globals) {
                    if (!visited.count(gl.get())) {
                        st.push(cur);
                        node = gl.get();
                        break;
                    }
                }
                if (node) continue;
                for (auto& f : pr->functions) {
                    if (!visited.count(f.get())) {
                        st.push(cur);
                        node = f.get();
                        break;
                    }
                }
                if (!node) {
                    visit(cur, result_stack, result_deque, visited);
                }
            } else if (auto f = dynamic_cast<Function*>(cur); f) {
                for (auto& s : f->body) {
                    if (!visited.count(s.get())) {
                        st.push(cur);
                        node = s.get();
                        break;
                    }
                }
                if (!node && !visited.count(cur)) {
                    visit(cur, result_stack, result_deque, visited);
                }
            } else if (auto pr = dynamic_cast<Print*>(cur); pr) {
                visit(cur, result_stack, result_deque, visited);
            } else if (auto fc = dynamic_cast<FunCall*>(cur); fc) {
                if (!visited.count(fc->func.get())) {
                    st.push(cur);
                    node = fc->func.get();
                } else {
                    for (auto& s : fc->args) {
                        if (!visited.count(s.get())) {
                            st.push(cur);
                            node = s.get();
                            break;
                        }
                    }
                    if (!node) {
                        visit(cur, result_stack, result_deque, visited);
                    }
                }
            } else if (auto ifthen = dynamic_cast<IfThenElse*>(cur); ifthen) {
                if (!visited.count(ifthen->then_branch.get())) {
                    st.push(cur);
                    node = ifthen->then_branch.get();
                } else if (!visited.count(ifthen->else_branch.get())) {
                    st.push(cur);
                    node = ifthen->else_branch.get();
                } else {
                    visit(cur, result_stack, result_deque, visited);
                }
            } else if (auto logic_op = dynamic_cast<LogicOp*>(cur); logic_op) {
                if (!visited.count(logic_op->lhs.get())) {
                    st.push(cur);
                    node = logic_op->lhs.get();
                } else if (!visited.count(logic_op->rhs.get())) {
                    st.push(cur);
                    node = logic_op->rhs.get();
                } else {
                    visit(cur, result_stack, result_deque, visited);
                }
            } else if (auto ret = dynamic_cast<Return*>(cur); ret) {
                visit(cur, result_stack, result_deque, visited);
            } else if (auto arith_op = dynamic_cast<ArithOp*>(cur); arith_op) {
                if (!visited.count(arith_op->lhs.get())) {
                    st.push(cur);
                    node = arith_op->lhs.get();
                } else if (!visited.count(arith_op->rhs.get())) {
                    st.push(cur);
                    node = arith_op->rhs.get();
                } else {
                    visit(cur, result_stack, result_deque, visited);
                }
            } else if (auto var_def = dynamic_cast<VarDef*>(cur); var_def) {
                visit(cur, result_stack, result_deque, visited);
            } else if (auto var = dynamic_cast<Var*>(cur); var) {
                visit(cur, result_stack, result_deque, visited);
            } else if (auto integer = dynamic_cast<Integer*>(cur); integer) {
                visit(cur, result_stack, result_deque, visited);
            } else if (auto ass = dynamic_cast<Assign*>(cur); ass) {
                visit(cur, result_stack, result_deque, visited);
            } else if (auto whil = dynamic_cast<While*>(cur); whil) {
                for (auto& s : whil->body) {
                    if (!visited.count(s.get())) {
                        node = s.get();
                        break;
                    }
                }
                if (!node) {
                    if (!visited.count(whil->condition.get())) {
                        node = whil->condition.get();
                    } else {
                        visit(cur, result_stack, result_deque, visited);
                    }
                }
            } else if (auto str = dynamic_cast<StringLiteral*>(cur); str) {
                visit(cur, result_stack, result_deque, visited);
            } else if (auto str = dynamic_cast<BoolLiteral*>(cur); str) {
                visit(cur, result_stack, result_deque, visited);
            } else if (auto arr = dynamic_cast<ArrayDeclaration*>(cur); arr) {
                visit(cur, result_stack, result_deque, visited);
            } else if (auto arr_ac = dynamic_cast<ArrayAccess*>(cur); arr_ac) {
                visit(cur, result_stack, result_deque, visited);
            } else if (auto arr_as = dynamic_cast<ArrayAssignment*>(cur);
                       arr_as) {
                if (!visited.count(arr_as->index.get())) {
                    st.push(cur);
                    node = arr_as->index.get();
                } else if (!visited.count(arr_as->value.get())) {
                    st.push(cur);
                    node = arr_as->value.get();
                }
                if (!node) {
                    visit(cur, result_stack, result_deque, visited);
                }
            } else {
                throw std::logic_error("err"s + typeid(cur).name());
            }
        }
    }

    assert(result_stack.empty());

    std::cout << "#include <iostream>\n\n";

    while (!result_deque.empty()) {
        std::cout << result_deque.front() << "\n\n";
        result_deque.pop_front();
    }

    return 0;
}

void visit(ASTNode* node, std::stack<std::string>& result_stack,
           std::deque<std::string>& result_deque,
           std::unordered_set<ASTNode*>& visited) {
    visited.insert(node);
    if (auto pr = dynamic_cast<Program*>(node); pr) {
        assert(result_stack.size() >= pr->globals.size());
        auto str = ""s;
        std::stack<std::string> temp_st;
        for (size_t i = 0; i < pr->globals.size(); ++i) {
            temp_st.push(result_stack.top());
            result_stack.pop();
        }
        for (size_t i = 0; i < pr->globals.size(); ++i) {
            if (i > 0) str += "\n";
            str += temp_st.top();
            temp_st.pop();
        }
        str += "\n";
        result_deque.push_front(str);
    } else if (auto f = dynamic_cast<Function*>(node); f) {
        auto str = f->return_type.to_string() + " " + f->name + "(";
        for (size_t i = 0; i < f->args.size(); ++i) {
            if (i > 0) str += ", ";
            str += f->args[i].type.to_string() + " " + f->args[i].name;
        }
        str += ") {\n";
        assert(result_stack.size() >= f->body.size());
        std::stack<std::string> temp_st;
        for (size_t i = 0; i < f->body.size(); ++i) {
            temp_st.push(result_stack.top());
            result_stack.pop();
        }
        for (size_t i = 0; i < f->body.size(); ++i) {
            if (i > 0) str += "\n";
            str += temp_st.top();
            temp_st.pop();
        }
        str += "\n}";
        result_deque.push_back(str);
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
        auto func = dynamic_cast<Function*>(fc->func.get());
        assert(func);
        auto str = func->name + "(";
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
        result_stack.push(var_def->type.to_string() + " " + var_def->name +
                          ";");
    } else if (auto var = dynamic_cast<Var*>(node); var) {
        result_stack.push(var->name);
    } else if (auto integer = dynamic_cast<Integer*>(node); integer) {
        result_stack.push(std::to_string(integer->val));
    } else if (auto ass = dynamic_cast<Assign*>(node); ass) {
        assert(result_stack.size() >= 1);
        auto expr = result_stack.top();
        result_stack.pop();
        result_stack.push(ass->var + " = " + expr + ";");
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
    } else if (auto str = dynamic_cast<StringLiteral*>(node); str) {
        result_stack.push("\"" + str->value + "\"");
    } else if (auto b_lit = dynamic_cast<BoolLiteral*>(node); b_lit) {
        result_stack.push(b_lit->value ? "true" : "false");
    } else if (auto arr = dynamic_cast<ArrayDeclaration*>(node); arr) {
        result_stack.push(arr->type.to_string() + " " + arr->name + "[" +
                          std::to_string(arr->type.array_size) + "];");
    } else if (auto arr_ac = dynamic_cast<ArrayAccess*>(node); arr_ac) {
        assert(result_stack.size() >= 1);
        auto expr = result_stack.top();
        result_stack.pop();
        result_stack.push(arr_ac->name + "[" + expr + "]");
    } else if (auto arr_as = dynamic_cast<ArrayAssignment*>(node); arr_as) {
        assert(result_stack.size() >= 2);
        auto value = result_stack.top();
        result_stack.pop();
        auto expr = result_stack.top();
        result_stack.pop();
        result_stack.push(arr_as->name + "[" + expr + "] = " + value + ";");
    } else {
        throw std::logic_error("err"s + typeid(node).name());
    }
}
