cmake_minimum_required(VERSION 3.0.0)
project(riscv-sim)

add_compile_options(-g)

add_subdirectory(src)

add_executable(memgen generate.cpp)
add_executable(verify verify.cpp)
add_executable(setup setup.cpp)

