cmake_minimum_required(VERSION 3.30.3)

project(vocabulary VERSION 0.0)

# set version of cpp
add_library(vocabulary_compiler_flags INTERFACE)
target_compile_features(vocabulary_compiler_flags INTERFACE cxx_std_17)

add_subdirectory(vocabulary)

add_executable(vocabulary.o main.cpp)

target_link_libraries(vocabulary.o PUBLIC
    gui
    vocabulary_compiler_flags
)

