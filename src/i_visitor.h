#pragma once

namespace AST {
struct ArithOp;
struct ArrayAccess;
struct ArrayDeclaration;
struct ArrayAssignment;
struct Assign;
struct ASTNode;
struct BoolLiteral;
struct FunCall;
struct Function;
struct IfThenElse;
struct Integer;
struct LogicOp;
struct Print;
struct Program;
struct Return;
struct StringLiteral;
struct Var;
struct VarDef;
struct While;
}  // namespace AST

class IASTVisitor {
   public:
    virtual ~IASTVisitor() = default;

    virtual void visit(AST::ArithOp* node) = 0;
    virtual void visit(AST::ArrayAccess* node) = 0;
    virtual void visit(AST::ArrayAssignment* node) = 0;
    virtual void visit(AST::ArrayDeclaration* node) = 0;
    virtual void visit(AST::Assign* node) = 0;
    virtual void visit(AST::BoolLiteral* node) = 0;
    virtual void visit(AST::FunCall* node) = 0;
    virtual void visit(AST::Function* node) = 0;
    virtual void visit(AST::IfThenElse* node) = 0;
    virtual void visit(AST::Integer* node) = 0;
    virtual void visit(AST::LogicOp* node) = 0;
    virtual void visit(AST::Print* node) = 0;
    virtual void visit(AST::Program* node) = 0;
    virtual void visit(AST::Return* node) = 0;
    virtual void visit(AST::StringLiteral* node) = 0;
    virtual void visit(AST::Var* node) = 0;
    virtual void visit(AST::VarDef* node) = 0;
    virtual void visit(AST::While* node) = 0;
};
