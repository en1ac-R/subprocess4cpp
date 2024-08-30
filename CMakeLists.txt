# https://cmake.org/cmake/help/latest/command/target_sources.html
#   Relative source file paths are interpreted as being relative to the current
#   source directory (i.e. CMAKE_CURRENT_SOURCE_DIR). See policy CMP0076.
cmake_minimum_required(VERSION 3.13)
project(subprocess)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "C++ standard required")
set(CMAKE_CXX_EXTENSIONS FALSE CACHE BOOL "C++ standard extension")

add_library(subprocess subprocess.h subprocess.cpp argv_buffer.h argv_buffer.cpp)
target_compile_options(
    subprocess
    PUBLIC
    -Wall -Werror -Wpedantic -Wextra -Wfatal-errors -pedantic --pedantic-errors
)

target_include_directories(
    subprocess PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)

find_package(GTest CONFIG REQUIRED) # GTest::gtest

add_executable(subprocess_ut main_ut.cpp subprocess_ut.cpp argv_buffer_ut.cpp)
target_link_libraries(subprocess_ut PRIVATE GTest::gtest subprocess)

add_executable(subprocess_capture_ut main_capture_ut.cpp)
target_link_libraries(subprocess_capture_ut PRIVATE subprocess)

add_executable(emit_output_mix emit_output_main_mix.cpp)
add_executable(emit_output_sep emit_output_main_sep.cpp)

enable_testing()
add_test(NAME subprocess_ut COMMAND subprocess_ut)

add_test(NAME subprocess_capture_mix COMMAND subprocess_capture_ut $<TARGET_FILE:emit_output_mix>)
add_test(NAME subprocess_capture_sep COMMAND subprocess_capture_ut $<TARGET_FILE:emit_output_sep>)
