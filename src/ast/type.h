#pragma once

#include <string>

enum class Type { Void, Int };

inline std::string to_string(Type type) {
    switch (type) {
        case Type::Void:
            return "void";
        case Type::Int:
            return "int";
        default:
            return "Unknown";
    }
}