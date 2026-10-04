#pragma once

#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "ast/ast.h"
#include "ast/type.h"
#include "call_stack.h"
#include "i_visitor.h"
#include "ir.h"

// Emits GNU (AT&T syntax) x86-64 assembly for the System V ABI.
//
// Simple stack-machine scheme: every expression leaves its value in %rax,
// intermediate values are pushed on the machine stack, every variable lives
// in memory (8-byte slots). Ints use 32-bit arithmetic and are kept
// sign-extended to 64 bits; bools are 0/1.
class X64Codegen : public IASTVisitor {
   public:
    explicit X64Codegen(std::ostream& os) : os_(os) {}

    void Generate(const IR& ir);

    void visit(AST::ArithOp* node) override;
    void visit(AST::ArrayAccess* node) override;
    void visit(AST::ArrayAssignment* node) override;
    void visit(AST::ArrayDeclaration* node) override;
    void visit(AST::Assign* node) override;
    void visit(AST::BoolLiteral* node) override;
    void visit(AST::FunCall* node) override;
    void visit(AST::Function* node) override;
    void visit(AST::IfThenElse* node) override;
    void visit(AST::Integer* node) override;
    void visit(AST::LogicOp* node) override;
    void visit(AST::Print* node) override;
    void visit(AST::Program* node) override;
    void visit(AST::Rand* node) override;
    void visit(AST::Return* node) override;
    void visit(AST::StringLiteral* node) override;
    void visit(AST::Var* node) override;
    void visit(AST::VarDef* node) override;
    void visit(AST::While* node) override;

   private:
    struct Storage {
        std::string addr;     // memory operand: "-8(%rbp)" or "gv_x(%rip)"
        AST::Type type;
        bool is_ref = false;  // slot holds a pointer to the array (params)
    };

    void DeclareVar(AST::ASTNode* node, const std::string& name,
                    AST::Type type);
    Storage& Lookup(AST::ASTNode* node, const std::string& name);
    void LoadArrayAddress(const Storage& storage, const std::string& reg);
    void EmitBoundsCheck(AST::ASTNode* node, const AST::Type& type);
    void EmitBlock(const AST::Statements& stmts);

    template <typename... Args>
    void Emit(Args... args) {
        *out_ << "    ";
        (*out_ << ... << args);
        *out_ << '\n';
    }
    void EmitLabel(const std::string& label) { *out_ << label << ":\n"; }
    void Push(const std::string& reg);
    void Pop(const std::string& reg);
    // Calls a function without stack arguments, keeping %rsp 16-byte aligned.
    void EmitCall(const std::string& name);

    std::string NewLabel();
    std::string AllocSlots(int count);

    template <typename... Args>
    [[noreturn]] void Error(AST::ASTNode* node, Args... args) {
        std::ostringstream oss;
        oss << node->loc << ": codegen: ";
        (oss << ... << args);
        throw std::runtime_error(oss.str());
    }

    std::ostream& os_;
    std::ostream* out_ = nullptr;  // where instructions currently go

    std::ostringstream text_;
    std::ostringstream rodata_;
    std::ostringstream bss_;

    CallStack<Storage> symbols_;
    AST::Function* current_function_ = nullptr;
    int frame_size_ = 0;  // bytes of locals in the current function
    int depth_ = 0;       // 8-byte values pushed on top of the frame
    int label_counter_ = 0;
    int string_counter_ = 0;
};
