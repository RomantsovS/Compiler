#pragma once

#include <memory>
#include <vector>

#include "type.h"

struct Block;
struct NameType;

struct ASTNode {
    virtual ~ASTNode() = default;
};

using Statements = std::vector<std::shared_ptr<ASTNode>>;
using Params = std::vector<NameType>;

// functions
std::shared_ptr<ASTNode> make_program(std::shared_ptr<Statements> functions);

std::shared_ptr<Statements> make_empty_function_list();

std::shared_ptr<Statements> append_function(
    std::shared_ptr<Statements> functions, std::shared_ptr<ASTNode> stmt);

std::shared_ptr<ASTNode> make_function(Type return_type,
                                       const std::string& name,
                                       std::shared_ptr<Params> params,
                                       std::shared_ptr<Statements> list);

std::shared_ptr<ASTNode> make_function_call(const std::string& name,
                                            std::shared_ptr<Statements> args);

std::shared_ptr<Statements> make_empty_arg_list();

std::shared_ptr<Statements> make_arg_list(std::shared_ptr<ASTNode> stmt);

std::shared_ptr<Statements> append_arg(std::shared_ptr<Statements> args,
                                       std::shared_ptr<ASTNode> stmt);

std::shared_ptr<Statements> make_empty_stmt_list();

std::shared_ptr<Statements> append_stmt(std::shared_ptr<Statements> list,
                                        std::shared_ptr<ASTNode> stmt);

std::shared_ptr<Params> make_empty_param_list();

std::shared_ptr<Params> make_param_list(Type type, const std::string& name);

std::shared_ptr<Params> append_param(std::shared_ptr<Params> params, Type type,
                                     const std::string& name);

std::shared_ptr<ASTNode> make_return(std::shared_ptr<ASTNode> stmt);

std::shared_ptr<ASTNode> make_decl(Type type, const std::string& name);

std::shared_ptr<ASTNode> make_integer(uint64_t value);

std::shared_ptr<ASTNode> make_assignment(const std::string& var,
                                         std::shared_ptr<ASTNode> st);

std::shared_ptr<ASTNode> make_print(std::shared_ptr<ASTNode> st);

std::shared_ptr<ASTNode> make_string_literal(const std::string& string);

std::shared_ptr<ASTNode> make_bool_literal(const bool value);

std::shared_ptr<ASTNode> make_var(const std::string& var);

std::shared_ptr<ASTNode> make_arith_op(const std::string& op,
                                       std::shared_ptr<ASTNode> lhs,
                                       std::shared_ptr<ASTNode> rhs);

std::shared_ptr<ASTNode> make_logic_op(const std::string& op,
                                       std::shared_ptr<ASTNode> lhs,
                                       std::shared_ptr<ASTNode> rhs);

std::shared_ptr<ASTNode> make_if(std::shared_ptr<ASTNode> condition,
                                 std::shared_ptr<ASTNode> then_branch,
                                 std::shared_ptr<ASTNode> else_branch);

std::shared_ptr<ASTNode> make_while(std::shared_ptr<ASTNode> condition,
                                    std::shared_ptr<ASTNode> statement);

std::shared_ptr<ASTNode> make_while(std::shared_ptr<ASTNode> condition,
                                    std::shared_ptr<Statements> statement);

std::shared_ptr<ASTNode> make_array_declaration(const std::string& name,
                                                const Type& type);

std::shared_ptr<ASTNode> make_array_access(const std::string& name,
                                           std::shared_ptr<ASTNode> expr);

std::shared_ptr<ASTNode> make_array_assignment(const std::string& name,
                                               std::shared_ptr<ASTNode> index,
                                               std::shared_ptr<ASTNode> value);
