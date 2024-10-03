cmake_minimum_required(VERSION 3.30.3)

project(vocabulary VERSION 0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set version of cpp
add_library(vocabulary_compiler_flags INTERFACE)
target_compile_features(vocabulary_compiler_flags INTERFACE cxx_std_20)

add_subdirectory(vocabulary)

add_executable(vcb main.cpp)

target_link_libraries(vcb PUBLIC
    gui
    vocabulary_compiler_flags
)

