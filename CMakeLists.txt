cmake_minimum_required(VERSION 3.28)
project(simd_lexer C CXX)

set(CMAKE_C_COMPILER "clang-20")
set(CMAKE_CXX_COMPILER "clang++-20")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse4.2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mbmi2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mpclmul")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mbmi2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mpclmul")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

include_directories(include)

add_library(simd_lex_library include/aligned_vector.hpp
                             src/CppTokenArray.cpp)

add_executable(ascii_determiner ascii_determiner.cpp)

add_executable(simd_lexer main.c
        lexer.c
        lexer.h
        tokens.h
        tokens.c
        print_utils.c
)
