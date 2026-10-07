cmake_minimum_required(VERSION 3.24)
project(cunaguaro LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(cunaguaro)
target_sources(cunaguaro PRIVATE src/main.cpp src/loader.cpp src/machine.cpp)
target_include_directories(cunaguaro PRIVATE inc)
target_compile_options(cunaguaro 
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
