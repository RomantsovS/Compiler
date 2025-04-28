#pragma once

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast/type.h"
#include "i_visitor.h"

struct SymbolEntry {
    AST::Type type;
};

class Symtable {
   public:
    Symtable() { scopes.push_back({}); }

    using ScopeMap = std::unordered_map<std::string, SymbolEntry>;

    bool Declare(const std::string& name, const SymbolEntry& entry) {
        if (scopes.back().count(name) != 0) {
            return false;
        }
        scopes.back()[name] = entry;
        return true;
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
    Symtable symtable;
};