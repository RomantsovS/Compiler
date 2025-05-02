#include "helpers.h"

#include <iostream>
#include <unordered_map>

#include "arithmetic_op.h"
#include "array.h"
#include "assign.h"
#include "bool_literal.h"
#include "expr.h"
#include "fun_call.h"
#include "function.h"
#include "if_then_else.h"
#include "integer.h"
#include "location.hh"
#include "logic_op.h"
#include "print.h"
#include "program.h"
#include "return.h"
#include "string_literal.h"
#include "var.h"
#include "while.h"

namespace AST {

std::unordered_map<std::string, std::shared_ptr<ASTNode>> name_to_func;

std::shared_ptr<ASTNode> make_program(
    std::shared_ptr<Statements> top_level_list) {
    std::cout << "create ast program " << '\n';
    auto node = std::make_shared<Program>();
    for (auto top_level : *top_level_list) {
        if (auto* func = dynamic_cast<Function*>(top_level.get()); func) {
            node->functions.push_back(top_level);
        } else {
            node->globals.push_back(top_level);
        }
    }
    return node;
}

std::shared_ptr<Statements> make_empty_top_level_list() {
    return std::make_shared<Statements>();
}

std::shared_ptr<Statements> append_top_level(
    std::shared_ptr<Statements> top_level_list, std::shared_ptr<ASTNode> stmt) {
    std::cout << "call ast append_function, functions initialized: "
              << static_cast<bool>(top_level_list) << '\n';
    if (!top_level_list) {
        top_level_list = make_empty_top_level_list();
    }
    top_level_list->push_back(stmt);
    return top_level_list;
}

std::shared_ptr<ASTNode> make_function(Type return_type,
                                       const std::string& name,
                                       std::shared_ptr<Params> params,
                                       std::shared_ptr<Statements> list) {
    std::cout << "create ast function " << name << '\n';
    auto node = std::make_shared<Function>();

    node->return_type = return_type;
    node->name = name;
    node->args = *params;
    node->body = *list;

    name_to_func[name] = node;

    return node;
}

std::shared_ptr<ASTNode> make_function_call(const std::string& name,
                                            std::shared_ptr<Statements> args) {
    std::cout << "create ast function call " << name << '\n';
    auto node = std::make_shared<FunCall>();

    node->name = name;
    node->args = *args;

    return node;
}

std::shared_ptr<Statements> make_empty_arg_list() {
    std::cout << "call ast make_empty_arg_list" << '\n';
    return std::make_shared<Statements>();
}

std::shared_ptr<Statements> make_arg_list(std::shared_ptr<ASTNode> stmt) {
    std::cout << "call ast make_arg_list" << '\n';
    auto args = make_empty_arg_list();
    args->push_back(stmt);
    return args;
}

std::shared_ptr<Statements> append_arg(std::shared_ptr<Statements> args,
                                       std::shared_ptr<ASTNode> stmt) {
    std::cout << "call ast append_arg, args initialized: "
              << static_cast<bool>(args) << '\n';
    if (!args) {
        args = make_empty_arg_list();
    }
    args->push_back(stmt);
    return args;
}

std::shared_ptr<Statements> make_empty_stmt_list() {
    std::cout << "call ast make_empty_stmt_list" << '\n';
    return std::make_shared<Statements>();
}

std::shared_ptr<Statements> append_stmt(std::shared_ptr<Statements> list,
                                        std::shared_ptr<ASTNode> stmt) {
    std::cout << "call ast append_stmt, list initialized: "
              << static_cast<bool>(list) << '\n';
    if (!list) {
        list = make_empty_stmt_list();
    }
    list->push_back(stmt);
    return list;
}

std::shared_ptr<Params> make_empty_param_list() {
    std::cout << "call ast make_empty_param_list" << '\n';
    return std::make_shared<Params>();
}

std::shared_ptr<Params> make_param_list(Type type, const std::string& name) {
    std::cout << "call ast make_param_list" << '\n';
    auto params = make_empty_param_list();
    params->push_back({name, type});
    return params;
}

std::shared_ptr<Params> append_param(std::shared_ptr<Params> params, Type type,
                                     const std::string& name) {
    std::cout << "call ast append_param, params initialized: "
              << static_cast<bool>(params) << '\n';
    if (!params) {
        params = make_empty_param_list();
    }
    params->push_back({name, type});
    return params;
}

std::shared_ptr<ASTNode> make_return(std::shared_ptr<ASTNode> stmt) {
    std::cout << "create ast return " << '\n';
    auto node = std::make_shared<Return>();
    node->statement = stmt;
    return node;
}

std::shared_ptr<ASTNode> make_decl(Type type, const std::string& name) {
    std::cout << "create ast decl " << name << '\n';
    auto node = std::make_shared<VarDef>();
    node->type = type;
    node->name = name;
    return node;
}

std::shared_ptr<ASTNode> make_integer(uint64_t value) {
    std::cout << "create ast integer " << value << '\n';
    auto node = std::make_shared<Integer>(value);
    return node;
}

std::shared_ptr<ASTNode> make_assignment(const std::string& var,
                                         std::shared_ptr<ASTNode> expr) {
    std::cout << "create ast assign " << var << '\n';
    auto node = std::make_shared<Assign>();
    node->var = var;
    node->expr = std::static_pointer_cast<Expr>(expr);
    return node;
}

std::shared_ptr<ASTNode> make_print(std::shared_ptr<ASTNode> st) {
    std::cout << "create ast print " << '\n';
    auto node = std::make_shared<Print>();
    node->st = st;
    return node;
}

std::shared_ptr<ASTNode> make_string_literal(const std::string& string) {
    std::cout << "create ast string literal " << string << '\n';
    auto node = std::make_shared<StringLiteral>();
    node->value = string;
    return node;
}

std::shared_ptr<ASTNode> make_bool_literal(const bool value) {
    std::cout << "create ast bool literal " << value << '\n';
    auto node = std::make_shared<BoolLiteral>(value);
    return node;
}

std::shared_ptr<ASTNode> make_var(const std::string& var) {
    std::cout << "create ast var " << var << '\n';
    auto node = std::make_shared<Var>();
    node->name = var;
    return node;
}

std::shared_ptr<ASTNode> make_arith_op(const std::string& op,
                                       std::shared_ptr<ASTNode> lhs,
                                       std::shared_ptr<ASTNode> rhs) {
    std::cout << "create ast arith_op " << '\n';
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

std::shared_ptr<ASTNode> make_logic_op(const std::string& op,
                                       std::shared_ptr<ASTNode> lhs,
                                       std::shared_ptr<ASTNode> rhs) {
    std::cout << "create ast logic_op " << '\n';
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

std::shared_ptr<ASTNode> make_if(std::shared_ptr<ASTNode> condition,
                                 std::shared_ptr<ASTNode> then_branch,
                                 std::shared_ptr<ASTNode> else_branch) {
    std::cout << "create ast if_then_else " << '\n';
    auto node = std::make_shared<IfThenElse>();
    node->condition = std::dynamic_pointer_cast<Expr>(condition);
    if (!node->condition) {
        throw std::runtime_error("lhs is not Expr node");
    }
    node->then_branch = then_branch;
    node->else_branch = else_branch;
    return node;
}

std::shared_ptr<ASTNode> make_while(std::shared_ptr<ASTNode> condition,
                                    std::shared_ptr<ASTNode> statement) {
    std::cout << "create ast while " << '\n';
    auto node = std::make_shared<While>();
    node->condition = condition;
    node->body.push_back(statement);
    return node;
}

std::shared_ptr<ASTNode> make_while(std::shared_ptr<ASTNode> condition,
                                    std::shared_ptr<Statements> list) {
    std::cout << "create ast while with block " << '\n';
    auto node = std::make_shared<While>();
    node->condition = condition;
    node->body = *list;
    return node;
}

std::shared_ptr<ASTNode> make_array_declaration(const std::string& name,
                                                const Type& type) {
    std::cout << "create ast array decl " << name << '\n';
    auto node = std::make_shared<ArrayDeclaration>();
    node->name = name;
    node->type = type;
    return node;
}

std::shared_ptr<ASTNode> make_array_access(const std::string& name,
                                           std::shared_ptr<ASTNode> expr) {
    std::cout << "create ast array access " << name << '\n';
    auto node = std::make_shared<ArrayAccess>();
    node->name = name;
    node->index = expr;
    return node;
}

std::shared_ptr<ASTNode> make_array_assignment(const std::string& name,
                                               std::shared_ptr<ASTNode> index,
                                               std::shared_ptr<ASTNode> value) {
    std::cout << "create ast array assignment " << name << '\n';
    auto node = std::make_shared<ArrayAssignment>();
    node->name = name;
    node->index = index;
    node->value = value;
    return node;
}

std::shared_ptr<ASTNode> with_location(std::shared_ptr<ASTNode> node,
                                       const EzAquarii::location& loc) {
    node->loc.line = loc.begin.line;
    node->loc.column = loc.begin.column;
    return node;
}

}  // namespace AST
