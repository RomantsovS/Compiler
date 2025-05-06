#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast/ast.h"
#include "call_stack.h"
#include "ir.h"

class Object;

class ObjectHolder {
   public:
    ObjectHolder() = default;

    template <typename T>
    static ObjectHolder Own(T&& object) {
        return ObjectHolder(std::make_shared<T>(std::forward<T>(object)));
    }
    static ObjectHolder None() { return {}; }

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

    template <typename T>
    const T* TryAs() const {
        return dynamic_cast<const T*>(this->Get());
    }

    explicit operator bool() const { return Get(); }

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
using Bool = ValueObject<bool>;

class ArrayObject : public Object {
   public:
    void Print(std::ostream& os) override {
        throw std::runtime_error("call ArrayObject Print");
    }

    const ObjectHolder GetObject(size_t i) const {
        if (i >= value.size()) {
            throw std::runtime_error("array access out of bounds");
        }
        return value[i];
    }

    void SetObject(size_t ind, ObjectHolder val) {
        value.resize(std::max(value.size(), ind + 1));
        value.at(ind) = val;
    }

   private:
    std::vector<ObjectHolder> value;
};

class Interpreter {
   public:
    Interpreter(const IR& ir, std::ostream& os) : ir_(ir), os_(os) {}

    int Exec();

   private:
    ObjectHolder Eval(std::shared_ptr<AST::ArithOp> node);
    ObjectHolder Eval(std::shared_ptr<AST::ArrayAccess> node);
    ObjectHolder Eval(std::shared_ptr<AST::ArrayAssignment> node);
    ObjectHolder Eval(std::shared_ptr<AST::ArrayDeclaration> node);
    ObjectHolder Eval(std::shared_ptr<AST::Assign> node);
    ObjectHolder Eval(std::shared_ptr<AST::ASTNode> node);
    ObjectHolder Eval(std::shared_ptr<AST::FunCall> node);
    ObjectHolder Eval(std::shared_ptr<AST::Function> node,
                      std::vector<ObjectHolder> args);
    ObjectHolder Eval(std::shared_ptr<AST::IfThenElse> node);
    ObjectHolder Eval(std::shared_ptr<AST::LogicOp> node);
    ObjectHolder Eval(std::shared_ptr<AST::Print> node);
    ObjectHolder Eval(std::shared_ptr<AST::Program> node);
    ObjectHolder Eval(std::shared_ptr<AST::Return> node);
    ObjectHolder Eval(std::shared_ptr<AST::Var> node);
    ObjectHolder Eval(std::shared_ptr<AST::VarDef> node);
    ObjectHolder Eval(std::shared_ptr<AST::While> node);

    template <typename... Args>
    void Error(AST::ASTNode* node, Args... args) {
        std::ostringstream oss;
        oss << node->loc;
        oss << ": ";
        (oss << ... << args);
        throw std::runtime_error(oss.str());
    }

    IR ir_;
    std::ostream& os_;

    CallStack<ObjectHolder> call_stack;
    std::unordered_map<std::string, std::shared_ptr<AST::Function>> functions;
};