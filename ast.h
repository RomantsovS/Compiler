#pragma once

#include <memory>
#include <vector>

#include "type.h"

struct Block;

struct ASTNode {
    virtual ~ASTNode() = default;
};

using Statements = std::vector<std::shared_ptr<ASTNode>>;

// functions
std::shared_ptr<ASTNode> make_function(Type return_type,
                                       const std::string& name,
                                       std::shared_ptr<Block> block);

std::shared_ptr<Block> make_block(std::shared_ptr<Statements> stmt_list);

std::shared_ptr<Statements> make_empty_stmt_list();

std::shared_ptr<Statements> append_stmt(std::shared_ptr<Statements> list,
                                        std::shared_ptr<ASTNode> stmt);

std::shared_ptr<ASTNode> make_decl(Type type, const std::string& name);

std::shared_ptr<ASTNode> make_integer(uint64_t value);

std::shared_ptr<ASTNode> make_assignment(const std::string& var,
                                         std::shared_ptr<ASTNode> st);

std::shared_ptr<ASTNode> make_print(std::shared_ptr<ASTNode> st);

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
