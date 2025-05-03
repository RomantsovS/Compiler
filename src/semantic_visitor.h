#pragma once

#include <algorithm>
#include <cassert>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast/ast.h"
#include "ast/type.h"
#include "i_visitor.h"

struct SymbolEntry {
    AST::Type type;
    AST::Location loc;
    std::vector<AST::Type> params;
};

class Symtable {
   public:
    Symtable() { scopes.push_back({}); }

    using ScopeMap = std::unordered_map<std::string, SymbolEntry>;

    SymbolEntry* Declare(const std::string& name, const SymbolEntry& entry) {
        if (auto iter = scopes.back().find(name); iter != scopes.back().end()) {
            return &iter->second;
        }
        scopes.back()[name] = entry;
        return nullptr;
    }

    void PushScope() { scopes.push_back({}); }
    void PopScope() {
        assert(!scopes.empty());
        scopes.pop_back();
    }

    SymbolEntry* Find(const std::string& name) {
        for (auto iter = scopes.rbegin(); iter != scopes.rend(); ++iter) {
            auto found = iter->find(name);
            if (found != iter->end()) return &found->second;
        }
        return nullptr;
    }

   private:
    std::vector<ScopeMap> scopes;
};

class SemanticVisitor : public IASTVisitor {
   public:
    void visit(AST::Program* node) override;
    void visit(AST::Function* node) override;
    void visit(AST::Print* node) override;
    void visit(AST::FunCall* node) override;
    void visit(AST::IfThenElse* node) override;
    void visit(AST::LogicOp* node) override;
    void visit(AST::Return* node) override;
    void visit(AST::ArithOp* node) override;
    void visit(AST::VarDef* node) override;
    void visit(AST::Var* node) override;
    void visit(AST::Integer* node) override;
    void visit(AST::Assign* node) override;
    void visit(AST::While* node) override;
    void visit(AST::StringLiteral* node) override;
    void visit(AST::BoolLiteral* node) override;
    void visit(AST::ArrayDeclaration* node) override;
    void visit(AST::ArrayAccess* node) override;
    void visit(AST::ArrayAssignment* node) override;

   private:
    template <typename... Args>
    void Error(AST::ASTNode* node, Args... args) {
        std::ostringstream oss;
        oss << node->loc;
        oss << ": ";
        (oss << ... << args);
        throw std::runtime_error(oss.str());
    }

    Symtable symtable;
};