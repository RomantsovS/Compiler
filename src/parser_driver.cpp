/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Krzysztof Narkiewicz <krzysztof.narkiewicz@ezaquarii.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "parser_driver.h"

#include <algorithm>
#include <sstream>

#include "ast/arithmetic_op.h"
#include "ast/array.h"
#include "ast/assign.h"
#include "ast/ast.h"
#include "ast/bool_literal.h"
#include "ast/expr.h"
#include "ast/fun_call.h"
#include "ast/function.h"
#include "ast/if_then_else.h"
#include "ast/integer.h"
#include "ast/logic_op.h"
#include "ast/name_type.h"
#include "ast/print.h"
#include "ast/program.h"
#include "ast/rand.h"
#include "ast/return.h"
#include "ast/string_literal.h"
#include "ast/var.h"
#include "ast/while.h"
#include "location.hh"

namespace EzAquarii {

using namespace AST;

ParserDriver::ParserDriver()
    : scanner_(*this), parser_(scanner_, *this, ast_) {}

int ParserDriver::parse() {
    // parser_.set_debug_level(1);
    return parser_.parse();
}

void ParserDriver::switchInputStream(std::istream* is) {
    scanner_.switch_streams(is, NULL);
}

void ParserDriver::ScannerAction(unsigned int length, std::string_view text) {
    loc_.step();
    loc_.columns(length);
    if (scanner_debug_level_ > 0) {
        std::cout << "Scanner: " << text << '\n'
                  << "increaseLocation(): " << loc_ << '\n';
    }
}

void ParserDriver::newline() { loc_.lines(1); }

const location& ParserDriver::get_location() const { return loc_; }

void ParserDriver::SetScannerDebugLevel(int level) {
    scanner_debug_level_ = level;
}

void ParserDriver::SetParserDebugLevel(int level) {
    parser_debug_level_ = level;
}

std::shared_ptr<AST::ASTNode> ParserDriver::GetAST() { return ast_; }

std::shared_ptr<ASTNode> ParserDriver::make_program(
    std::shared_ptr<Statements> top_level_list) {
    ParserLog("create ast program ", '\n');
    auto node = std::make_shared<Program>();
    for (auto top_level : *top_level_list) {
        node->globals.push_back(top_level);
    }
    return node;
}

std::shared_ptr<Statements> ParserDriver::make_empty_top_level_list() {
    return std::make_shared<Statements>();
}

std::shared_ptr<Statements> ParserDriver::append_top_level(
    std::shared_ptr<Statements> top_level_list, std::shared_ptr<ASTNode> stmt) {
    ParserLog("call ast append_function, functions initialized: ",
              static_cast<bool>(top_level_list), '\n');
    if (!top_level_list) {
        top_level_list = make_empty_top_level_list();
    }
    top_level_list->push_back(stmt);
    return top_level_list;
}

std::shared_ptr<ASTNode> ParserDriver::make_function(
    Type return_type, const std::string& name, std::shared_ptr<Params> params,
    std::shared_ptr<Statements> list) {
    ParserLog("create ast function ", name, '\n');
    auto node = std::make_shared<Function>();

    node->return_type = return_type;
    node->name = name;
    node->args = *params;
    node->body = *list;

    return node;
}

std::shared_ptr<ASTNode> ParserDriver::make_function_call(
    const std::string& name, std::shared_ptr<Statements> args) {
    ParserLog("create ast function call ", name, '\n');
    auto node = std::make_shared<FunCall>();

    node->name = name;

    node->args.resize(args->size());
    std::transform(args->begin(), args->end(), node->args.begin(),
                   [](const std::shared_ptr<ASTNode>& arg) {
                       auto expr = std::dynamic_pointer_cast<Expr>(arg);
                       if (!expr) {
                           throw std::runtime_error("arg is not Expr node");
                       }
                       return expr;
                   });

    return node;
}

std::shared_ptr<Statements> ParserDriver::make_empty_arg_list() {
    ParserLog("call ast ParserDriver::make_empty_arg_list", '\n');
    return std::make_shared<Statements>();
}

std::shared_ptr<Statements> ParserDriver::make_arg_list(
    std::shared_ptr<ASTNode> stmt) {
    ParserLog("call ast ParserDriver::make_arg_list", '\n');
    auto args = make_empty_arg_list();
    args->push_back(stmt);
    return args;
}

std::shared_ptr<Statements> ParserDriver::append_arg(
    std::shared_ptr<Statements> args, std::shared_ptr<ASTNode> stmt) {
    ParserLog("call ast append_arg, args initialized: ",
              static_cast<bool>(args), '\n');
    if (!args) {
        args = make_empty_arg_list();
    }
    args->push_back(stmt);
    return args;
}

std::shared_ptr<Statements> ParserDriver::make_empty_stmt_list() {
    ParserLog("call ast ParserDriver::make_empty_stmt_list", '\n');
    return std::make_shared<Statements>();
}

std::shared_ptr<Statements> ParserDriver::append_stmt(
    std::shared_ptr<Statements> list, std::shared_ptr<ASTNode> stmt) {
    ParserLog("call ast append_stmt, list initialized: ",
              static_cast<bool>(list), '\n');
    if (!list) {
        list = make_empty_stmt_list();
    }
    list->push_back(stmt);
    return list;
}

std::shared_ptr<Params> ParserDriver::make_empty_param_list() {
    ParserLog("call ast ParserDriver::make_empty_param_list", '\n');
    return std::make_shared<Params>();
}

std::shared_ptr<Params> ParserDriver::make_param_list(Type type,
                                                      const std::string& name) {
    ParserLog("call ast ParserDriver::make_param_list", '\n');
    auto params = ParserDriver::make_empty_param_list();
    params->push_back({name, type});
    return params;
}

std::shared_ptr<Params> ParserDriver::append_param(
    std::shared_ptr<Params> params, Type type, const std::string& name) {
    ParserLog("call ast append_param, params initialized: ",
              static_cast<bool>(params), '\n');
    if (!params) {
        params = make_empty_param_list();
    }
    params->push_back({name, type});
    return params;
}

std::shared_ptr<ASTNode> ParserDriver::make_return(
    std::shared_ptr<ASTNode> expr) {
    ParserLog("create ast return ", '\n');
    auto node = std::make_shared<Return>();
    if (expr) {
        node->expr = std::dynamic_pointer_cast<Expr>(expr);
        if (!node->expr) {
            throw std::runtime_error("expr is not Expr node");
        }
    }
    return node;
}

std::shared_ptr<AST::ASTNode> ParserDriver::make_rand() {
    ParserLog("create ast rand ", '\n');
    return std::make_shared<Rand>();
}

std::shared_ptr<ASTNode> ParserDriver::make_decl(Type type,
                                                 const std::string& name) {
    ParserLog("create ast decl ", name, '\n');
    auto node = std::make_shared<VarDef>();
    node->type = type;
    node->name = name;
    return node;
}

std::shared_ptr<ASTNode> ParserDriver::make_integer(uint64_t value) {
    ParserLog("create ast integer ", value, '\n');
    auto node = std::make_shared<Integer>(value);
    return node;
}

std::shared_ptr<ASTNode> ParserDriver::make_assignment(
    const std::string& var, std::shared_ptr<ASTNode> expr) {
    ParserLog("create ast assign ", var, '\n');
    auto node = std::make_shared<Assign>();
    node->var = var;
    node->expr = std::dynamic_pointer_cast<Expr>(expr);
    if (!node->expr) {
        throw std::runtime_error("expr is not Expr node");
    }
    return node;
}

std::shared_ptr<ASTNode> ParserDriver::make_print(
    std::shared_ptr<ASTNode> expr) {
    ParserLog("create ast print ", '\n');
    auto node = std::make_shared<Print>();
    node->expr = expr;
    return node;
}

std::shared_ptr<ASTNode> ParserDriver::make_string_literal(
    const std::string& string) {
    ParserLog("create ast string literal ", string, '\n');
    auto node = std::make_shared<StringLiteral>();
    node->value = string;
    return node;
}

std::shared_ptr<ASTNode> ParserDriver::make_bool_literal(const bool value) {
    ParserLog("create ast bool literal ", value, '\n');
    auto node = std::make_shared<BoolLiteral>(value);
    return node;
}

std::shared_ptr<ASTNode> ParserDriver::make_var(const std::string& var) {
    ParserLog("create ast var ", var, '\n');
    auto node = std::make_shared<Var>();
    node->name = var;
    return node;
}

std::shared_ptr<ASTNode> ParserDriver::make_arith_op(
    const std::string& op, std::shared_ptr<ASTNode> lhs,
    std::shared_ptr<ASTNode> rhs) {
    ParserLog("create ast arith_op ", '\n');
    auto node = std::make_shared<ArithOp>();
    node->lhs = std::dynamic_pointer_cast<Expr>(lhs);
    if (!node->lhs) {
        throw std::runtime_error("lhs is not Expr node");
    }
    node->op = op;
    node->rhs = std::static_pointer_cast<Expr>(rhs);
    if (!node->rhs) {
        throw std::runtime_error("rhs is not Expr node");
    }

    return node;
}

std::shared_ptr<ASTNode> ParserDriver::make_logic_op(
    const std::string& op, std::shared_ptr<ASTNode> lhs,
    std::shared_ptr<ASTNode> rhs) {
    ParserLog("create ast logic_op ", '\n');
    auto node = std::make_shared<LogicOp>();
    node->lhs = std::dynamic_pointer_cast<Expr>(lhs);
    if (!node->lhs) {
        throw std::runtime_error("lhs is not Expr node");
    }
    node->op = op;
    node->rhs = std::static_pointer_cast<Expr>(rhs);
    if (!node->rhs) {
        throw std::runtime_error("rhs is not Expr node");
    }

    return node;
}

std::shared_ptr<ASTNode> ParserDriver::make_if(
    std::shared_ptr<ASTNode> condition, std::shared_ptr<ASTNode> then_branch,
    std::shared_ptr<ASTNode> else_branch) {
    ParserLog("create ast if_then_else ", '\n');
    auto node = std::make_shared<IfThenElse>();
    node->condition = std::dynamic_pointer_cast<Expr>(condition);
    if (!node->condition) {
        throw std::runtime_error("lhs is not Expr node");
    }
    node->then_branch.push_back(then_branch);
    if (else_branch) {
        node->else_branch.push_back(else_branch);
    }
    return node;
}

std::shared_ptr<ASTNode> ParserDriver::make_if(
    std::shared_ptr<ASTNode> condition, std::shared_ptr<Statements> then_branch,
    std::shared_ptr<Statements> else_branch) {
    ParserLog("create ast if_then_else ", '\n');
    auto node = std::make_shared<IfThenElse>();
    node->condition = std::dynamic_pointer_cast<Expr>(condition);
    if (!node->condition) {
        throw std::runtime_error("lhs is not Expr node");
    }
    node->then_branch = *then_branch;
    node->else_branch = *else_branch;
    return node;
}

std::shared_ptr<ASTNode> ParserDriver::make_while(
    std::shared_ptr<ASTNode> condition, std::shared_ptr<ASTNode> statement) {
    ParserLog("create ast while ", '\n');
    auto node = std::make_shared<While>();
    node->condition = condition;
    node->body.push_back(statement);
    return node;
}

std::shared_ptr<ASTNode> ParserDriver::make_while(
    std::shared_ptr<ASTNode> condition, std::shared_ptr<Statements> list) {
    ParserLog("create ast while with block ", '\n');
    auto node = std::make_shared<While>();
    node->condition = condition;
    node->body = *list;
    return node;
}

std::shared_ptr<ASTNode> ParserDriver::make_array_declaration(
    const std::string& name, const Type& type) {
    ParserLog("create ast array decl ", name, '\n');
    auto node = std::make_shared<ArrayDeclaration>();
    node->name = name;
    node->type = type;
    return node;
}

std::shared_ptr<ASTNode> ParserDriver::make_array_access(
    const std::string& name, std::shared_ptr<ASTNode> expr) {
    ParserLog("create ast array access ", name, '\n');
    auto node = std::make_shared<ArrayAccess>();
    node->name = name;
    node->index = expr;
    return node;
}

std::shared_ptr<ASTNode> ParserDriver::make_array_assignment(
    const std::string& name, std::shared_ptr<ASTNode> index,
    std::shared_ptr<ASTNode> expr) {
    ParserLog("create ast array assignment ", name, '\n');
    auto node = std::make_shared<ArrayAssignment>();
    node->name = name;
    node->index = index;
    node->expr = std::dynamic_pointer_cast<Expr>(expr);
    if (!node->expr) {
        throw std::runtime_error("expr is not Expr node");
    }
    return node;
}

std::shared_ptr<ASTNode> ParserDriver::with_location(
    std::shared_ptr<ASTNode> node, const EzAquarii::location& loc) {
    node->loc.line = loc.begin.line;
    node->loc.column = loc.begin.column;
    return node;
}

}  // namespace EzAquarii
