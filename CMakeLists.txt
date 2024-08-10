cmake_minimum_required(VERSION 3.22)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake")
include(sft_library)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/cmake_toolbox")
include(CMakeToolboxInitialize)

CMakeToolboxInitialize()

SetCAndCXXStandard(11 20)

EnableCCache()

project(sft)

enable_testing()
include(GoogleTest)
include(CTest)

add_subdirectory(third_party/sdl EXCLUDE_FROM_ALL)
add_subdirectory(third_party/tinyobjloader EXCLUDE_FROM_ALL)
add_subdirectory(third_party/googletest EXCLUDE_FROM_ALL)
set(GOOGLETEST_PATH "third_party/googletest" EXCLUDE_FROM_ALL)
set(BENCHMARK_ENABLE_TESTING FALSE)
add_subdirectory(third_party/googlebenchmark EXCLUDE_FROM_ALL)
add_subdirectory(third_party/glm EXCLUDE_FROM_ALL)
set(MARL_BUILD_TESTS FALSE)
set(MARL_BUILD_BENCHMARKS FALSE)
set(MARL_BUILD_EXAMPLES FALSE)
add_subdirectory(third_party/marl EXCLUDE_FROM_ALL)
add_subdirectory(third_party/superliminal EXCLUDE_FROM_ALL)

add_subdirectory(src)
