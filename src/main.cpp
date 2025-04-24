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
#include "ast/return.h"
#include "ast/string_literal.h"
#include "ast/type.h"
#include "ast/var.h"
#include "ast/while.h"
#include "interpreter.h"
#include "parser.hpp"
#include "print_visitor.h"
#include "scanner.h"

int main() {
    bool use_parser = true;
    std::shared_ptr<AST::ASTNode> ast;
    if (use_parser) {
        EzAquarii::Interpreter interpreter(ast);
        auto res = interpreter.parse();
        if (res) return res;
    } else {
        auto prog = std::make_shared<AST::Program>();

        auto abs_fun = std::make_shared<AST::Function>();
        prog->functions.push_back(abs_fun);

        abs_fun->name = "abs";
        abs_fun->args = {{"x", AST::Type::Int()}};

        {
            auto st = std::make_shared<AST::IfThenElse>();
            auto l_op = std::make_shared<AST::LogicOp>();
            st->condition = l_op;
            {
                l_op->op = "<";
                auto lhs = std::make_shared<AST::Var>();
                lhs->name = "x";
                l_op->lhs = lhs;
                auto rhs = std::make_shared<AST::Integer>();
                rhs->val = 0;
                l_op->rhs = rhs;
            }

            {
                auto st2 = std::make_shared<AST::ArithOp>();
                st2->op = "-";
                auto lhs = std::make_shared<AST::Integer>();
                lhs->val = 0;
                st2->lhs = lhs;
                auto rhs = std::make_shared<AST::Var>();
                rhs->name = "x";
                st2->rhs = rhs;
                auto ret = std::make_shared<AST::Return>();
                ret->statement = st2;
                st->then_branch = ret;
            }
            {
                auto var = std::make_shared<AST::Var>();
                var->name = "x";
                auto ret = std::make_shared<AST::Return>();
                ret->statement = var;
                st->else_branch = ret;
            }
            abs_fun->body.push_back(st);
        }
        abs_fun->return_type = AST::Type::Int();

        auto sum_fun = std::make_shared<AST::Function>();
        prog->functions.push_back(sum_fun);

        sum_fun->name = "sum";
        sum_fun->args = {{"x", AST::Type::Int()}, {"y", AST::Type::Int()}};
        sum_fun->return_type = AST::Type::Int();

        {
            auto var = std::make_shared<AST::VarDef>();
            var->name = "z";
            var->type = AST::Type::Int();
            sum_fun->body.push_back(var);

            auto ass = std::make_shared<AST::Assign>();
            ass->var = "z";
            auto three = std::make_shared<AST::Integer>();
            three->val = 3;
            ass->st = three;
            sum_fun->body.push_back(ass);

            auto whil = std::make_shared<AST::While>();
            {
                auto st4 = std::make_shared<AST::LogicOp>();
                st4->op = "<";
                auto lhs = std::make_shared<AST::Var>();
                lhs->name = "z";
                st4->lhs = lhs;
                auto rhs = std::make_shared<AST::Integer>();
                rhs->val = 20;
                st4->rhs = rhs;
                whil->condition = st4;
            }

            auto ass2 = std::make_shared<AST::Assign>();
            ass2->var = "z";

            auto st2 = std::make_shared<AST::ArithOp>();
            st2->op = "+";
            auto lhs = std::make_shared<AST::Var>();
            lhs->name = "z";
            st2->lhs = lhs;
            {
                auto st3 = std::make_shared<AST::ArithOp>();
                st3->op = "+";
                auto lhs2 = std::make_shared<AST::Var>();
                lhs2->name = "x";
                st3->lhs = lhs2;
                auto rhs = std::make_shared<AST::Var>();
                rhs->name = "y";
                st3->rhs = rhs;
                st2->rhs = st3;
            }
            ass2->st = st2;

            whil->body.push_back(ass2);

            sum_fun->body.push_back(whil);

            auto ret = std::make_shared<AST::Return>();
            auto v = std::make_shared<AST::Var>();
            v->name = "z";
            ret->statement = v;
            sum_fun->body.push_back(ret);
        }

        auto main_fun = std::make_shared<AST::Function>();
        prog->functions.push_back(main_fun);

        main_fun->name = "main";

        auto abs_fun_call = std::make_shared<AST::FunCall>();
        abs_fun_call->name = abs_fun->name;

        {
            auto st2 = std::make_shared<AST::ArithOp>();
            st2->op = "+";
            auto lhs = std::make_shared<AST::Integer>();
            lhs->val = -10;
            st2->lhs = lhs;

            auto sum_fun_call = std::make_shared<AST::FunCall>();
            sum_fun_call->name = sum_fun->name;
            {
                auto one = std::make_shared<AST::Integer>();
                one->val = 1;
                auto two = std::make_shared<AST::Integer>();
                two->val = 2;
                sum_fun_call->args.push_back(one);
                sum_fun_call->args.push_back(two);
            }
            st2->rhs = sum_fun_call;
            abs_fun_call->args.push_back(st2);
        }

        auto print = std::make_shared<AST::Print>();
        print->st = abs_fun_call;

        main_fun->body.push_back(print);
        main_fun->return_type = AST::Type::Int();
        ast = prog;
    }
    auto* node = ast.get();
    std::stack<AST::ASTNode*> st;

    std::unordered_set<AST::ASTNode*> visited;

    PrintVisitor print_visitor;

    while (!st.empty() || node) {
        if (node) {
            assert(st.empty() || st.top() != node);
            st.push(node);
            auto* cur = node;
            node = nullptr;
            if (auto pr = dynamic_cast<AST::Program*>(cur); pr) {
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
            } else if (auto f = dynamic_cast<AST::Function*>(cur); f) {
                for (auto& s : f->body) {
                    if (!visited.count(s.get())) {
                        node = s.get();
                        break;
                    }
                }
            } else if (auto pr = dynamic_cast<AST::Print*>(cur); pr) {
                node = pr->st.get();
            } else if (auto fc = dynamic_cast<AST::FunCall*>(cur); fc) {
                ;
            } else if (auto ifthen = dynamic_cast<AST::IfThenElse*>(cur);
                       ifthen) {
                if (!visited.count(ifthen->condition.get()))
                    node = ifthen->condition.get();
                else if (!visited.count(ifthen->then_branch.get()))
                    node = ifthen->then_branch.get();
                else if (!visited.count(ifthen->else_branch.get()))
                    node = ifthen->else_branch.get();
            } else if (auto logic_op = dynamic_cast<AST::LogicOp*>(cur);
                       logic_op) {
                if (!visited.count(logic_op->lhs.get()))
                    node = logic_op->lhs.get();
                else if (!visited.count(logic_op->rhs.get()))
                    node = logic_op->rhs.get();
            } else if (auto ret = dynamic_cast<AST::Return*>(cur); ret) {
                node = ret->statement.get();
            } else if (auto arith_op = dynamic_cast<AST::ArithOp*>(cur);
                       arith_op) {
                if (!visited.count(arith_op->lhs.get()))
                    node = arith_op->lhs.get();
                else if (!visited.count(arith_op->rhs.get()))
                    node = arith_op->rhs.get();
            } else if (auto var_def = dynamic_cast<AST::VarDef*>(cur);
                       var_def) {
                ;
            } else if (auto var = dynamic_cast<AST::Var*>(cur); var) {
                ;
            } else if (auto integer = dynamic_cast<AST::Integer*>(cur);
                       integer) {
                ;
            } else if (auto ass = dynamic_cast<AST::Assign*>(cur); ass) {
                if (!visited.count(ass->st.get())) node = ass->st.get();
            } else if (auto whil = dynamic_cast<AST::While*>(cur); whil) {
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
            } else if (auto str = dynamic_cast<AST::StringLiteral*>(cur); str) {
                ;
            } else if (auto str = dynamic_cast<AST::BoolLiteral*>(cur); str) {
                ;
            } else if (auto arr = dynamic_cast<AST::ArrayDeclaration*>(cur);
                       arr) {
                ;
            } else if (auto arr_ac = dynamic_cast<AST::ArrayAccess*>(cur);
                       arr_ac) {
                if (!visited.count(arr_ac->index.get())) {
                    node = arr_ac->index.get();
                }
            } else if (auto arr_as = dynamic_cast<AST::ArrayAssignment*>(cur);
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
            if (auto pr = dynamic_cast<AST::Program*>(cur); pr) {
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
                    cur->accept(&print_visitor);
                    visited.insert(cur);
                }
            } else if (auto f = dynamic_cast<AST::Function*>(cur); f) {
                for (auto& s : f->body) {
                    if (!visited.count(s.get())) {
                        st.push(cur);
                        node = s.get();
                        break;
                    }
                }
                if (!node && !visited.count(cur)) {
                    cur->accept(&print_visitor);
                    visited.insert(cur);
                }
            } else if (auto pr = dynamic_cast<AST::Print*>(cur); pr) {
                cur->accept(&print_visitor);
                visited.insert(cur);
            } else if (auto fc = dynamic_cast<AST::FunCall*>(cur); fc) {
                for (auto& s : fc->args) {
                    if (!visited.count(s.get())) {
                        st.push(cur);
                        node = s.get();
                        break;
                    }
                }
                if (!node) {
                    cur->accept(&print_visitor);
                    visited.insert(cur);
                }
            } else if (auto ifthen = dynamic_cast<AST::IfThenElse*>(cur);
                       ifthen) {
                if (!visited.count(ifthen->then_branch.get())) {
                    st.push(cur);
                    node = ifthen->then_branch.get();
                } else if (!visited.count(ifthen->else_branch.get())) {
                    st.push(cur);
                    node = ifthen->else_branch.get();
                } else {
                    cur->accept(&print_visitor);
                    visited.insert(cur);
                }
            } else if (auto logic_op = dynamic_cast<AST::LogicOp*>(cur);
                       logic_op) {
                if (!visited.count(logic_op->lhs.get())) {
                    st.push(cur);
                    node = logic_op->lhs.get();
                } else if (!visited.count(logic_op->rhs.get())) {
                    st.push(cur);
                    node = logic_op->rhs.get();
                } else {
                    cur->accept(&print_visitor);
                    visited.insert(cur);
                }
            } else if (auto ret = dynamic_cast<AST::Return*>(cur); ret) {
                cur->accept(&print_visitor);
                visited.insert(cur);
            } else if (auto arith_op = dynamic_cast<AST::ArithOp*>(cur);
                       arith_op) {
                if (!visited.count(arith_op->lhs.get())) {
                    st.push(cur);
                    node = arith_op->lhs.get();
                } else if (!visited.count(arith_op->rhs.get())) {
                    st.push(cur);
                    node = arith_op->rhs.get();
                } else {
                    cur->accept(&print_visitor);
                    visited.insert(cur);
                }
            } else if (auto var_def = dynamic_cast<AST::VarDef*>(cur);
                       var_def) {
                cur->accept(&print_visitor);
                visited.insert(cur);
            } else if (auto var = dynamic_cast<AST::Var*>(cur); var) {
                cur->accept(&print_visitor);
                visited.insert(cur);
            } else if (auto integer = dynamic_cast<AST::Integer*>(cur);
                       integer) {
                cur->accept(&print_visitor);
                visited.insert(cur);
            } else if (auto ass = dynamic_cast<AST::Assign*>(cur); ass) {
                cur->accept(&print_visitor);
                visited.insert(cur);
            } else if (auto whil = dynamic_cast<AST::While*>(cur); whil) {
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
                        cur->accept(&print_visitor);
                        visited.insert(cur);
                    }
                }
            } else if (auto str = dynamic_cast<AST::StringLiteral*>(cur); str) {
                cur->accept(&print_visitor);
                visited.insert(cur);
            } else if (auto str = dynamic_cast<AST::BoolLiteral*>(cur); str) {
                cur->accept(&print_visitor);
                visited.insert(cur);
            } else if (auto arr = dynamic_cast<AST::ArrayDeclaration*>(cur);
                       arr) {
                cur->accept(&print_visitor);
                visited.insert(cur);
            } else if (auto arr_ac = dynamic_cast<AST::ArrayAccess*>(cur);
                       arr_ac) {
                cur->accept(&print_visitor);
                visited.insert(cur);
            } else if (auto arr_as = dynamic_cast<AST::ArrayAssignment*>(cur);
                       arr_as) {
                if (!visited.count(arr_as->index.get())) {
                    st.push(cur);
                    node = arr_as->index.get();
                } else if (!visited.count(arr_as->value.get())) {
                    st.push(cur);
                    node = arr_as->value.get();
                }
                if (!node) {
                    cur->accept(&print_visitor);
                    visited.insert(cur);
                }
            } else {
                throw std::logic_error("err"s + typeid(cur).name());
            }
        }
    }

    print_visitor.Print(std::cout);

    return 0;
}
