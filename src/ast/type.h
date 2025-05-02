#pragma once

#include <cassert>
#include <string>
#include <utility>

namespace AST {

enum class BaseType { Unknown, Void, Int, Bool };

struct Type {
    BaseType base = BaseType::Unknown;
    bool is_array = false;
    int array_size = 0;  // optional: 0 = unknown or dynamic size

    std::string to_string() {
        switch (base) {
            case BaseType::Void:
                return "void";
            case BaseType::Int:
                return "int";
            case BaseType::Bool:
                return "bool";
            default:
                return "Unknown";
        }
    }

    Type GetArrayBase() {
        assert(base == BaseType::Int);
        return Int();
    }

    static Type Void() { return {BaseType::Void}; }
    static Type Int() { return {BaseType::Int}; }
    static Type Bool() { return {BaseType::Bool}; }
    static Type IntArray(int size) { return {BaseType::Int, true, size}; }
};

inline bool operator==(const Type& lhs, const Type& rhs) {
    return std::tuple(lhs.base, lhs.is_array, lhs.array_size) ==
           std::tuple(rhs.base, rhs.is_array, rhs.array_size);
}

inline bool operator!=(const Type& lhs, const Type& rhs) {
    return !(lhs == rhs);
}

}  // namespace AST
