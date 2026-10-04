#include "x64_codegen.h"

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
#include "ast/var.h"
#include "ast/while.h"

namespace {

const char* const kArgRegs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
constexpr size_t kNumArgRegs = 6;

std::string FunctionLabel(const std::string& name) { return "fn_" + name; }
std::string GlobalLabel(const std::string& name) { return "gv_" + name; }

std::string EscapeAsciiDirective(const std::string& value) {
    static const char* const kOctal = "01234567";
    std::string result;
    for (unsigned char c : value) {
        if (c == '"' || c == '\\') {
            result += '\\';
            result += c;
        } else if (c >= 0x20 && c < 0x7f) {
            result += c;
        } else {
            result += '\\';
            result += kOctal[(c >> 6) & 7];
            result += kOctal[(c >> 3) & 7];
            result += kOctal[c & 7];
        }
    }
    return result;
}

int SlotCount(const AST::Type& type) {
    return type.is_array ? std::max(type.array_size, 1) : 1;
}

}  // namespace

void X64Codegen::Generate(const IR& ir) {
    out_ = &text_;
    ir.GetAST()->accept(this);

    os_ << "    .text\n" << text_.str();
    os_ << "\n    .section .rodata\n" << rodata_.str();
    os_ << "\n    .bss\n" << bss_.str();
    os_ << "\n    .section .note.GNU-stack,\"\",@progbits\n";
}

std::string X64Codegen::NewLabel() {
    return ".L" + std::to_string(label_counter_++);
}

std::string X64Codegen::AllocSlots(int count) {
    frame_size_ += 8 * count;
    return std::to_string(-frame_size_) + "(%rbp)";
}

void X64Codegen::Push(const std::string& reg) {
    Emit("pushq ", reg);
    ++depth_;
}

void X64Codegen::Pop(const std::string& reg) {
    Emit("popq ", reg);
    --depth_;
}

void X64Codegen::EmitCall(const std::string& name) {
    bool pad = depth_ % 2 != 0;
    if (pad) Emit("subq $8, %rsp");
    Emit("call ", name);
    if (pad) Emit("addq $8, %rsp");
}

void X64Codegen::DeclareVar(AST::ASTNode* node, const std::string& name,
                            AST::Type type) {
    Storage storage{"", type};
    if (!current_function_) {
        auto label = GlobalLabel(name);
        bss_ << "    .align 8\n"
             << label << ":\n    .zero " << 8 * SlotCount(type) << '\n';
        storage.addr = label + "(%rip)";
    } else {
        storage.addr = AllocSlots(SlotCount(type));
        if (type.is_array) {
            Emit("leaq ", storage.addr, ", %rdi");
            Emit("movq $", SlotCount(type), ", %rcx");
            Emit("xorl %eax, %eax");
            Emit("rep stosq");
        } else {
            Emit("movq $0, ", storage.addr);
        }
    }
    if (symbols_.Declare(name, storage)) {
        Error(node, "redeclaration of ", name);
    }
}

X64Codegen::Storage& X64Codegen::Lookup(AST::ASTNode* node,
                                        const std::string& name) {
    auto* storage = symbols_.Find(name);
    if (!storage) Error(node, "undeclared variable ", name);
    return *storage;
}

void X64Codegen::LoadArrayAddress(const Storage& storage,
                                  const std::string& reg) {
    if (storage.is_ref) {
        Emit("movq ", storage.addr, ", ", reg);
    } else {
        Emit("leaq ", storage.addr, ", ", reg);
    }
}

// Expects the index in %rax; leaves it there.
void X64Codegen::EmitBoundsCheck(AST::ASTNode* node, const AST::Type& type) {
    if (type.array_size <= 0) return;
    auto ok = NewLabel();
    Emit("cmpq $", type.array_size, ", %rax");
    Emit("jb ", ok);  // unsigned compare also rejects negative indices
    Emit("movq %rax, %rdi");
    Emit("movq $", type.array_size, ", %rsi");
    Emit("movq $", node->loc.line, ", %rdx");
    Emit("movq $", node->loc.column, ", %rcx");
    EmitCall("rt_out_of_bounds@PLT");
    EmitLabel(ok);
}

void X64Codegen::EmitBlock(const AST::Statements& stmts) {
    symbols_.PushScope();
    for (auto& stmt : stmts) stmt->accept(this);
    symbols_.PopScope();
}

void X64Codegen::visit(AST::Program* node) {
    for (auto& global : node->globals) global->accept(this);
}

void X64Codegen::visit(AST::Function* node) {
    std::ostringstream body;
    out_ = &body;
    current_function_ = node;
    frame_size_ = 0;
    depth_ = 0;

    symbols_.PushScope();
    for (size_t i = 0; i < node->args.size(); ++i) {
        const auto& param = node->args[i];
        Storage storage{"", param.type, param.type.is_array};
        if (i < kNumArgRegs) {
            storage.addr = AllocSlots(1);
            Emit("movq ", kArgRegs[i], ", ", storage.addr);
        } else {
            // Stack arguments sit above the return address and saved %rbp.
            storage.addr =
                std::to_string(16 + 8 * (i - kNumArgRegs)) + "(%rbp)";
        }
        if (symbols_.Declare(param.name, storage)) {
            Error(node, "redefinition of parameter ", param.name);
        }
    }
    for (auto& stmt : node->body) stmt->accept(this);
    symbols_.PopScope();

    out_ = &text_;
    current_function_ = nullptr;

    auto label = FunctionLabel(node->name);
    *out_ << '\n';
    EmitLabel(label);
    if (node->name == "main") {
        Emit(".globl main");
        EmitLabel("main");
    }
    Emit("pushq %rbp");
    Emit("movq %rsp, %rbp");
    int frame = (frame_size_ + 15) / 16 * 16;
    if (frame) Emit("subq $", frame, ", %rsp");
    *out_ << body.str();
    Emit("xorl %eax, %eax");  // falling off the end returns 0
    EmitLabel(".Lret_" + label);
    Emit("leave");
    Emit("ret");
}

void X64Codegen::visit(AST::VarDef* node) {
    DeclareVar(node, node->name, node->type);
}

void X64Codegen::visit(AST::ArrayDeclaration* node) {
    DeclareVar(node, node->name, node->type);
}

void X64Codegen::visit(AST::Var* node) {
    auto& storage = Lookup(node, node->name);
    if (storage.type.is_array) {
        LoadArrayAddress(storage, "%rax");
    } else {
        Emit("movq ", storage.addr, ", %rax");
    }
}

void X64Codegen::visit(AST::Assign* node) {
    auto& storage = Lookup(node, node->var);
    if (storage.type.is_array) {
        Error(node, "assigning whole arrays is not supported");
    }
    node->expr->accept(this);
    Emit("movq %rax, ", storage.addr);
}

void X64Codegen::visit(AST::ArrayAccess* node) {
    auto& storage = Lookup(node, node->name);
    if (!storage.type.is_array) Error(node, node->name, " is not an array");
    node->index->accept(this);
    EmitBoundsCheck(node, storage.type);
    LoadArrayAddress(storage, "%rcx");
    Emit("movq (%rcx,%rax,8), %rax");
}

void X64Codegen::visit(AST::ArrayAssignment* node) {
    auto& storage = Lookup(node, node->name);
    if (!storage.type.is_array) Error(node, node->name, " is not an array");
    node->index->accept(this);
    EmitBoundsCheck(node, storage.type);
    Push("%rax");
    node->expr->accept(this);
    Pop("%rcx");
    LoadArrayAddress(storage, "%rdx");
    Emit("movq %rax, (%rdx,%rcx,8)");
}

void X64Codegen::visit(AST::Integer* node) {
    Emit("movq $", node->value, ", %rax");
}

void X64Codegen::visit(AST::BoolLiteral* node) {
    Emit("movq $", node->value ? 1 : 0, ", %rax");
}

void X64Codegen::visit(AST::StringLiteral* node) {
    auto label = ".Lstr" + std::to_string(string_counter_++);
    rodata_ << label << ":\n    .asciz \"" << EscapeAsciiDirective(node->value)
            << "\"\n";
    Emit("leaq ", label, "(%rip), %rax");
}

void X64Codegen::visit(AST::Rand* node) {
    EmitCall("rand@PLT");
    Emit("cltq");
}

void X64Codegen::visit(AST::ArithOp* node) {
    node->lhs->accept(this);
    Push("%rax");
    node->rhs->accept(this);
    Emit("movq %rax, %rcx");
    Pop("%rax");

    if (node->op == "+") {
        Emit("addl %ecx, %eax");
    } else if (node->op == "-") {
        Emit("subl %ecx, %eax");
    } else if (node->op == "*") {
        Emit("imull %ecx, %eax");
    } else if (node->op == "/" || node->op == "%") {
        auto ok = NewLabel();
        Emit("testl %ecx, %ecx");
        Emit("jnz ", ok);
        Emit("movq $", node->loc.line, ", %rdi");
        Emit("movq $", node->loc.column, ", %rsi");
        EmitCall("rt_div_by_zero@PLT");
        EmitLabel(ok);
        Emit("cltd");
        Emit("idivl %ecx");
        if (node->op == "%") Emit("movl %edx, %eax");
    } else {
        Error(node, "unknown arith op ", node->op);
    }
    Emit("cltq");
}

void X64Codegen::visit(AST::LogicOp* node) {
    node->lhs->accept(this);
    Push("%rax");
    node->rhs->accept(this);
    Emit("movq %rax, %rcx");
    Pop("%rax");

    Emit("cmpq %rcx, %rax");
    if (node->op == "<") {
        Emit("setl %al");
    } else if (node->op == ">") {
        Emit("setg %al");
    } else if (node->op == "==") {
        Emit("sete %al");
    } else {
        Error(node, "unknown logic op ", node->op);
    }
    Emit("movzbq %al, %rax");
}

void X64Codegen::visit(AST::FunCall* node) {
    size_t n = node->args.size();
    size_t n_stack = n > kNumArgRegs ? n - kNumArgRegs : 0;
    size_t n_reg = n - n_stack;

    // Reserve the outgoing stack-argument area (plus padding above it so that
    // %rsp is 16-byte aligned at the call), then evaluate args left to right.
    int pad = (depth_ + n_stack) % 2;
    int area = 8 * static_cast<int>(n_stack + pad);
    if (area) Emit("subq $", area, ", %rsp");
    depth_ += n_stack + pad;

    for (size_t i = 0; i < n; ++i) {
        node->args[i]->accept(this);
        if (i < kNumArgRegs) {
            Push("%rax");
        } else {
            int offset = 8 * static_cast<int>(n_reg + i - kNumArgRegs);
            Emit("movq %rax, ", offset, "(%rsp)");
        }
    }
    for (size_t i = n_reg; i > 0; --i) Pop(kArgRegs[i - 1]);

    Emit("call ", FunctionLabel(node->name));
    if (area) Emit("addq $", area, ", %rsp");
    depth_ -= n_stack + pad;
}

void X64Codegen::visit(AST::Return* node) {
    if (node->expr) node->expr->accept(this);
    Emit("jmp .Lret_", FunctionLabel(current_function_->name));
}

void X64Codegen::visit(AST::Print* node) {
    auto expr = std::dynamic_pointer_cast<AST::Expr>(node->expr);
    if (!expr) Error(node, "print argument is not an expression");

    expr->accept(this);
    Emit("movq %rax, %rdi");
    if (expr->node_type == AST::NodeType::StringLiteral) {
        EmitCall("rt_print_str@PLT");
    } else if (!expr->type.is_array &&
               (expr->type.base == AST::BaseType::Int ||
                expr->type.base == AST::BaseType::Bool)) {
        EmitCall("rt_print_int@PLT");
    } else {
        Error(node, "cannot print value of type ", expr->type);
    }
}

void X64Codegen::visit(AST::IfThenElse* node) {
    auto else_label = NewLabel();
    auto end_label = NewLabel();

    node->condition->accept(this);
    Emit("testq %rax, %rax");
    Emit("jz ", else_label);
    EmitBlock(node->then_branch);
    Emit("jmp ", end_label);
    EmitLabel(else_label);
    EmitBlock(node->else_branch);
    EmitLabel(end_label);
}

void X64Codegen::visit(AST::While* node) {
    auto cond_label = NewLabel();
    auto end_label = NewLabel();

    EmitLabel(cond_label);
    node->condition->accept(this);
    Emit("testq %rax, %rax");
    Emit("jz ", end_label);
    EmitBlock(node->body);
    Emit("jmp ", cond_label);
    EmitLabel(end_label);
}
