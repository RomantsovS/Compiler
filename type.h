#pragma once

#include <string>

enum class Type {
	VOID,
	INT
};

inline std::string to_string(Type type) {
    switch (type) {
        case Type::VOID:
            return "void";
        case Type::INT:
            return "int";
        default:
            return "Unknown";
    }
}