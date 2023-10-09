cmake_minimum_required(VERSION 3.22)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake")
include(sft_library)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/cmake_toolbox")
include(CMakeToolboxInitialize)

CMakeToolboxInitialize()

SetCAndCXXStandard(11 20)

EnableCCache()

project(sft)

add_subdirectory(third_party/sdl)
add_subdirectory(third_party/tinyobjloader)
add_subdirectory(third_party/googletest)
set(GOOGLETEST_PATH "third_party/googletest")
set(BENCHMARK_ENABLE_TESTING FALSE)
add_subdirectory(third_party/googlebenchmark)
add_subdirectory(third_party/glm)
set(MARL_BUILD_TESTS FALSE)
set(MARL_BUILD_BENCHMARKS FALSE)
set(MARL_BUILD_EXAMPLES FALSE)
add_subdirectory(third_party/marl)
add_subdirectory(third_party/superliminal)

add_subdirectory(src)
