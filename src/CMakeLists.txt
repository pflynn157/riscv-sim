
cmake_minimum_required(VERSION 3.0.0)
project(riscv-sim)

set(SRC
    cpu.cpp
    debug.cpp
    decode.cpp
    main.cpp
    memory.cpp
)

add_executable(cpu ${SRC})
