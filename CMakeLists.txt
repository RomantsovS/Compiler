cmake_minimum_required (VERSION 3.8)

project ("Compiler")

add_executable (compiler "main.cpp" "Compiler.h")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    set(MY_COMLILE_OPTIONS /W4 /WX)
endif()

target_compile_options(compiler PRIVATE ${MY_COMLILE_OPTIONS})