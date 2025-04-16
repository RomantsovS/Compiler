#include "ast.h"

#include <iostream>

#include "function.h"
#include "var.h"

std::shared_ptr<ASTNode> make_function(const std::string& name) {
    std::cout << "create ast function " << name << '\n';
    auto node = std::make_shared<Function>();
    return node;
}

std::shared_ptr<ASTNode> make_decl(const std::string& name) {
    std::cout << "create ast decl " << name << '\n';
    auto node = std::make_shared<VarDef>();
    return node;
}
