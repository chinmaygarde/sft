if(__sft_library)
  return()
endif()
set(__sft_library INCLUDED)

macro(sft_library LIBRARY_NAME_ARG)

add_library(${LIBRARY_NAME_ARG} ${ARGN})

target_include_directories(${LIBRARY_NAME_ARG} PUBLIC .)

endmacro()

macro(sft_executable EXECUTABLE_NAME_ARG)

add_executable(${EXECUTABLE_NAME_ARG} ${ARGN})

endmacro()
