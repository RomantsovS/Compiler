include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(unit_tests
  parser_tests.cpp
  semantic_analysis_tests.cpp
)

target_include_directories(unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_include_directories(unit_tests PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../)
target_link_libraries(unit_tests gtest_main compiler_frontend compiler_lib)

include(GoogleTest)
gtest_discover_tests(unit_tests)
