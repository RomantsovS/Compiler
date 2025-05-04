#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "i_visitor.h"

class Object;

class ObjectHolder {
   public:
    ObjectHolder() = default;

    template <typename T>
    static ObjectHolder Own(T&& object) {
        return ObjectHolder(std::make_shared<T>(std::forward<T>(object)));
    }

    Object& operator*() { return *Get(); }

    const Object& operator*() const { return *Get(); }

    Object* operator->() { return Get(); }

    const Object* operator->() const { return Get(); }

    Object* Get() { return data.get(); }

    const Object* Get() const { return data.get(); }

    template <typename T>
    T* TryAs() {
        return dynamic_cast<T*>(this->Get());
    }

    // template <typename T>
    // const T* TryAs() const {
    //     return dynamic_cast<const T*>(this->Get());
    // }

   private:
    ObjectHolder(std::shared_ptr<Object> new_data)
        : data(std::move(new_data)) {}

    std::shared_ptr<Object> data;
};

class Object {
   public:
    virtual ~Object() = default;
    virtual void Print(std::ostream& os) = 0;
};

template <typename T>
class ValueObject : public Object {
   public:
    ValueObject(T v) : value(v) {}

    void Print(std::ostream& os) override { os << value; }

    const T& GetValue() const { return value; }

   private:
    T value;
};

using Number = ValueObject<int>;

class ArrayObject : public Object {
   public:
    void Print(std::ostream& os) override {
        throw std::runtime_error("call ArrayObject Print");
    }

    const ObjectHolder GetObject(size_t i) const { return value[i]; }
    void SetObject(size_t ind, ObjectHolder val) {
        value.resize(std::max(value.size(), ind + 1));
        value.at(ind) = val;
    }

   private:
    std::vector<ObjectHolder> value;
};

class InterpreterVisitor : public IASTVisitor {
   public:
    InterpreterVisitor(std::ostream& os) : os_(os) {}

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
    ObjectHolder Eval(std::shared_ptr<AST::ASTNode> node);
    ObjectHolder Eval(std::shared_ptr<AST::Var> node);
    ObjectHolder Eval(std::shared_ptr<AST::ArrayAccess> node);

    std::ostream& os_;

    std::unordered_map<std::string, ObjectHolder> variables;
};