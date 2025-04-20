#pragma once

#include <memory>
#include <vector>

#include "type.h"

struct Block;

struct ASTNode {
    virtual ~ASTNode() = default;
};

// functions
std::shared_ptr<ASTNode> make_function(Type return_type,
                                       const std::string& name,
                                       std::shared_ptr<Block> block);

std::shared_ptr<Block> make_block(
    std::shared_ptr<std::vector<std::shared_ptr<ASTNode>>> stmt_list);

std::shared_ptr<std::vector<std::shared_ptr<ASTNode>>> make_empty_stmt_list();

std::shared_ptr<std::vector<std::shared_ptr<ASTNode>>> append_stmt(
    std::shared_ptr<std::vector<std::shared_ptr<ASTNode>>> list,
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