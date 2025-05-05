#pragma once

#include <string>
#include <unordered_map>

#include "ast/function.h"
#include "ast/type.h"

struct SymbolInfo {
    AST::Type type;
};

class IR {
   public:
    void AddSymbol(const std::string& name, const SymbolInfo symbol_info) {
        symbols_[name] = symbol_info;
    }

    void AddFunction(const std::string& name,
                     std::shared_ptr<AST::Function> func) {
        functions_[name] = func;
    }

    std::shared_ptr<AST::Function> GetFunction(const std::string& name) const {
        return functions_.at(name);
    }

    void SetAST(std::shared_ptr<AST::ASTNode> ast) { ast_ = ast; }
    std::shared_ptr<AST::ASTNode> GetAST() const { return ast_; }

   private:
    std::unordered_map<std::string, SymbolInfo> symbols_;
    std::unordered_map<std::string, std::shared_ptr<AST::Function>> functions_;
    std::shared_ptr<AST::ASTNode> ast_;
};
