cmake_minimum_required (VERSION 3.8)

project ("Compiler")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable (compiler "main.cpp"  "function.h" "print.h" "return.h"
                         "assign.h" "while.h" "if_then_else.h" "arithmetic_op.h" "logic_op.h"
                         "integer.h" "boolean.h" "string.h" "var.h" "fun_call.h" "statement.h" "types.h")

if (MSVC)
    set(MY_COMLILE_OPTIONS /W4 /WX)
endif()

target_compile_options(compiler PRIVATE ${MY_COMLILE_OPTIONS})