
cmake_minimum_required(VERSION 3.0.0)
project(riscv-sim)

set(SRC
    main.cpp
)

add_executable(cpu ${SRC})
