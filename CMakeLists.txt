cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(sft)

add_subdirectory(third_party/sdl)

add_executable(sft main.cc)

target_link_libraries(sft SDL2)
