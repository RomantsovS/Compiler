#include "ast.h"

#include <iostream>

#include "assign.h"
#include "block.h"
#include "function.h"
#include "integer.h"
#include "print.h"
#include "var.h"

std::shared_ptr<ASTNode> make_function(Type return_type,
                                       const std::string& name,
                                       std::shared_ptr<Block> block) {
    std::cout << "create ast function " << name << '\n';
    auto node = std::make_shared<Function>();

    node->return_type = return_type;
    node->name = name;
    node->body = *block.get();

    return node;
}

std::shared_ptr<Block> make_block(
    std::shared_ptr<std::vector<std::shared_ptr<ASTNode>>> stmt_list) {
    auto block = std::make_shared<Block>();
    block->body = *stmt_list.get();
    return block;
}

std::shared_ptr<std::vector<std::shared_ptr<ASTNode>>> append_stmt(
    std::shared_ptr<std::vector<std::shared_ptr<ASTNode>>> list,
    std::shared_ptr<ASTNode> stmt) {
    if (!list) {
        list = std::make_shared<std::vector<std::shared_ptr<ASTNode>>>();
    }
    list->push_back(stmt);
    return list;
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
    auto node = std::make_shared<Integer>();
    node->val = value;
    return node;
}

std::shared_ptr<ASTNode> make_assignment(const std::string& var,
                                         std::shared_ptr<ASTNode> st) {
    std::cout << "create ast assign " << var << '\n';
    auto node = std::make_shared<Assign>();
    node->var = var;
    node->st = st;
    return node;
}

std::shared_ptr<ASTNode> make_print(std::shared_ptr<ASTNode> st) {
    std::cout << "create ast print " << '\n';
    auto node = std::make_shared<Print>();
    node->st = st;
    return node;
}

std::shared_ptr<ASTNode> make_var(const std::string& var) {
    std::cout << "create ast var " << var << '\n';
    auto node = std::make_shared<Var>();
    node->name = var;
    return node;
}