#Minimum version of CMake to build this project
cmake_minimum_required(VERSION 3.8)

# Declaration et nommage du projet
project(parser)

set(CXX "clang++")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS "-Wall -Werror -pedantic")

find_package(BISON)
find_package(FLEX)

bison_target(Parser src/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
flex_target(Scanner src/scanner.l ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp)

add_flex_bison_dependency(Scanner Parser)

add_library(compiler_frontend
  STATIC
  ${FLEX_Scanner_OUTPUTS}
  ${BISON_Parser_OUTPUTS}
)

target_include_directories(compiler_frontend PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(compiler_frontend PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

add_library(compiler_lib
  "src/parser_driver.cpp"
  "src/pretty_print_visitor.cpp"
  "src/print_visitor.cpp"
  "src/semantic_visitor.cpp"
  "src/driver.cpp"
  "src/interpreter.cpp"
)

target_include_directories(compiler_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(compiler_lib PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

add_executable(compiler "src/main.cpp")

target_link_libraries(compiler compiler_frontend compiler_lib)

enable_testing()
add_subdirectory(test)

add_subdirectory(test_programs)
