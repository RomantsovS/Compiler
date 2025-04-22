#pragma once

#include <string>

enum class BaseType { Void, Int };

struct Type {
    BaseType base;
    bool is_array = false;
    int array_size = 0;  // optional: 0 = unknown or dynamic size

    std::string to_string() {
        switch (base) {
            case BaseType::Void:
                return "void";
            case BaseType::Int:
                return "int";
            default:
                return "Unknown";
        }
    }

    static Type Int() { return {BaseType::Int}; }
    static Type IntArray(int size) { return {BaseType::Int, true, size}; }
};
