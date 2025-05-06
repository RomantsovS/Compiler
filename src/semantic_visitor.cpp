#include "semantic_visitor.h"

#include <algorithm>

#include "ast/arithmetic_op.h"
#include "ast/array.h"
#include "ast/assign.h"
#include "ast/bool_literal.h"
#include "ast/fun_call.h"
#include "ast/function.h"
#include "ast/if_then_else.h"
#include "ast/integer.h"
#include "ast/logic_op.h"
#include "ast/print.h"
#include "ast/program.h"
#include "ast/rand.h"
#include "ast/return.h"
#include "ast/string_literal.h"
#include "ast/type.h"
#include "ast/var.h"
#include "ast/while.h"

void SemanticVisitor::visit(AST::ArithOp* node) {
    node->lhs->accept(this);
    node->rhs->accept(this);

    if (node->lhs->type.base == AST::BaseType::Unknown ||
        node->lhs->type != node->rhs->type) {
        Error(node, "Type mismatch: cannot perform ", node->op, " for ",
              node->lhs->type, " and ", node->rhs->type);
    }

    node->type = node->lhs->type;
}

void SemanticVisitor::visit(AST::ArrayAccess* node) {
    auto* entry = symtable.Find(node->name);
    if (!entry) {
        Error(node, "Undeclared array variable ", node->name);
    }
    node->type = entry->type.GetArrayBase();

    node->index->accept(this);

    if (auto index = std::dynamic_pointer_cast<AST::Integer>(node->index);
        index) {
        if (index->value < 0 || index->value >= entry->type.array_size) {
            Error(node, "Array assign out of bounds");
        }
    }
}

void SemanticVisitor::visit(AST::ArrayAssignment* node) {
    auto* entry = symtable.Find(node->name);
    if (!entry) {
        Error(node, "Undeclared array variable ", node->name);
    }
    node->index->accept(this);

    if (auto index = std::dynamic_pointer_cast<AST::Integer>(node->index);
        index) {
        if (index->value < 0 || index->value >= entry->type.array_size) {
            Error(node, "Array assign out of bounds");
        }
    }

    node->expr->accept(this);

    if (node->expr->type.is_array ||
        entry->type.base != node->expr->type.base) {
        Error(node, "Type mismatch: cannot assign ", node->expr->type, " to ",
              node->name);
    }
}

void SemanticVisitor::visit(AST::ArrayDeclaration* node) {
    if (auto* entry = symtable.Declare(node->name, {node->type, node->loc});
        entry) {
        Error(node, "Redeclaration of ", node->name,
              ". Previously declared at ", entry->loc);
    }
    ir_->AddSymbol(node->name, {node->type});
}

void SemanticVisitor::visit(AST::Assign* node) {
    auto* entry = symtable.Find(node->var);
    if (!entry) {
        Error(node, "Undeclared variable ", node->var);
    }
    node->expr->accept(this);
    if (entry->type != node->expr->type) {
        Error(node, "Type mismatch: cannot assign ", node->expr->type, " to ",
              node->var);
    }
}

void SemanticVisitor::visit(AST::BoolLiteral* node) {}

void SemanticVisitor::visit(AST::FunCall* node) {
    auto* entry = symtable.Find(node->name);
    if (!entry) {
        Error(node, "Undeclared func ", node->name);
    }
    node->type = entry->type;

    if (node->args.size() != entry->params.size()) {
        Error(node, "Incorrect arguments number to call ", node->name,
              ". Expected ", entry->params.size(), " but got ",
              node->args.size());
    }

    for (size_t i = 0; i < node->args.size(); ++i) {
        node->args[i]->accept(this);
        if (node->args[i]->type != entry->params[i]) {
            Error(node, "Incorrect argument ", i, " type to call ", node->name,
                  ". Expected ", entry->params[i], " but got ",
                  node->args[i]->type);
        }
    }
}

void SemanticVisitor::visit(AST::Function* node) {
    std::vector<AST::Type> params(node->args.size());
    std::transform(
        node->args.begin(), node->args.end(), params.begin(),
        [](const AST::NameType name_type) { return name_type.type; });
    if (auto* entry = symtable.Declare(node->name,
                                       {node->return_type, node->loc, params});
        entry) {
        Error(node, "Redeclaration of ", node->name,
              ". Previously declared at ", entry->loc);
    }
    ir_->AddFunction(node->name, std::dynamic_pointer_cast<AST::Function>(
                                     node->shared_from_this()));

    symtable.PushScope();

    for (size_t i = 0; i < node->args.size(); ++i) {
        if (auto* entry = symtable.Declare(node->args[i].name,
                                           {node->args[i].type, node->loc});
            entry) {
            Error(node, "Redeclaration of ", node->args[i].name,
                  ". Previously declared at ", entry->loc);
        }
    }

    bool has_return = false;
    for (auto stmt : node->body) {
        stmt->accept(this);

        if (std::dynamic_pointer_cast<AST::Function>(stmt)) {
            Error(node, "Nested function definition is prohibited");
        }
        if (auto return_node = std::dynamic_pointer_cast<AST::Return>(stmt);
            return_node) {
            has_return = true;
            if (return_node->expr->type != node->return_type) {
                Error(return_node.get(), "Type mismatch: ", node->name,
                      " return type is ", node->return_type, " but got ",
                      return_node->expr->type);
            }
        }
    }

    if (node->return_type.base != AST::BaseType::Void && !has_return) {
        Error(node, "non-void function ", node->name,
              " does not return a value");
    }

    symtable.PopScope();
}

void SemanticVisitor::visit(AST::IfThenElse* node) {
    node->condition->accept(this);

    if (node->condition->type.is_array ||
        node->condition->type.base != AST::BaseType::Bool) {
        Error(node, "Type mismatch: condition is not bool");
    }

    node->then_branch->accept(this);
    if (node->else_branch) {
        node->else_branch->accept(this);
    }
}

void SemanticVisitor::visit(AST::Integer* node) {}

void SemanticVisitor::visit(AST::LogicOp* node) {
    node->lhs->accept(this);
    node->rhs->accept(this);

    if (node->lhs->type.base == AST::BaseType::Unknown ||
        node->lhs->type != node->rhs->type) {
        Error(node, "Type mismatch: cannot perform ", node->op, " for ",
              node->lhs->type, " and ", node->rhs->type);
    }
}

void SemanticVisitor::visit(AST::Print* node) { node->expr->accept(this); }

void SemanticVisitor::visit(AST::Program* node) {
    for (auto global : node->globals) {
        global->accept(this);
    }
    bool has_main = false;
    for (auto function : node->functions) {
        if (auto func = std::dynamic_pointer_cast<AST::Function>(function);
            func->name == "main") {
            has_main = true;
        }
        function->accept(this);
    }
    if (!has_main) {
        Error(node, "Main function isn't found");
    }
}

void SemanticVisitor::visit(AST::Rand* node) {}

void SemanticVisitor::visit(AST::Return* node) { node->expr->accept(this); }

void SemanticVisitor::visit(AST::StringLiteral* node) {}

void SemanticVisitor::visit(AST::Var* node) {
    auto* entry = symtable.Find(node->name);
    if (!entry) {
        Error(node, "Undeclared variable ", node->name);
    }
    node->type = entry->type;
}

void SemanticVisitor::visit(AST::VarDef* node) {
    if (auto* entry = symtable.Declare(node->name, {node->type, node->loc});
        entry) {
        Error(node, "Redeclaration of ", node->name,
              ". Previously declared at ", entry->loc);
    }
    ir_->AddSymbol(node->name, {node->type});
}

void SemanticVisitor::visit(AST::While* node) {
    node->condition->accept(this);

    symtable.PushScope();

    for (size_t i = 0; i < node->body.size(); ++i) {
        node->body[i]->accept(this);
    }

    symtable.PopScope();
}
