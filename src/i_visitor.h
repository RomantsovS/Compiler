#pragma once

namespace AST {
struct ASTNode;
struct Program;
struct Function;
struct Print;
struct FunCall;
struct IfThenElse;
struct LogicOp;
struct Return;
struct ArithOp;
struct VarDef;
struct Var;
struct Integer;
struct Assign;
struct While;
struct StringLiteral;
struct BoolLiteral;
struct ArrayDeclaration;
struct ArrayAccess;
struct ArrayAssignment;
}  // namespace AST

class IASTVisitor {
   public:
    virtual ~IASTVisitor() = default;

    virtual void visit(AST::Program* node) = 0;
    virtual void visit(AST::Function* node) = 0;
    virtual void visit(AST::Print* node) = 0;
    virtual void visit(AST::FunCall* node) = 0;
    virtual void visit(AST::IfThenElse* node) = 0;
    virtual void visit(AST::LogicOp* node) = 0;
    virtual void visit(AST::Return* node) = 0;
    virtual void visit(AST::ArithOp* node) = 0;
    virtual void visit(AST::VarDef* node) = 0;
    virtual void visit(AST::Var* node) = 0;
    virtual void visit(AST::Integer* node) = 0;
    virtual void visit(AST::Assign* node) = 0;
    virtual void visit(AST::While* node) = 0;
    virtual void visit(AST::StringLiteral* node) = 0;
    virtual void visit(AST::BoolLiteral* node) = 0;
    virtual void visit(AST::ArrayDeclaration* node) = 0;
    virtual void visit(AST::ArrayAccess* node) = 0;
    virtual void visit(AST::ArrayAssignment* node) = 0;
};
