cmake_minimum_required(VERSION 3.24)
project(iss LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(iss-sim)
target_sources(iss-sim PRIVATE src/main.cpp)
target_include_directories(iss-sim PRIVATE inc)
target_compile_options(iss-sim 
    PRIVATE 
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Werror
)
