
if(__shaders)
  return()
endif()
set(__shaders INCLUDED)

function(shader TARGET_TO_ADD_TO SOURCE_SHADER)
  get_filename_component(SOURCE_SHADER_PATH ${SOURCE_SHADER} ABSOLUTE)
  get_filename_component(SOURCE_SHADER_NAME ${SOURCE_SHADER} NAME)
  string(REPLACE "." "_" SOURCE_SHADER_NAME ${SOURCE_SHADER_NAME})

  set(CC_OUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/gen/shaders/${SOURCE_SHADER_NAME}.gen.cc)
  set(SPV_OUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/gen/shaders/${SOURCE_SHADER_NAME}.gen.spv)

  add_custom_command(
    OUTPUT ${CC_OUT_PATH}
    COMMAND glslangValidator ${SOURCE_SHADER_PATH} -g -G -o ${SPV_OUT_PATH}
    COMMAND spirv-cross ${SPV_OUT_PATH} --cpp --cpp-interface-name ${SOURCE_SHADER_NAME}_interface --output ${CC_OUT_PATH}
    DEPENDS ${SOURCE_SHADER} glslangValidator spirv-cross
  )
  target_sources(${TARGET_TO_ADD_TO}
    PRIVATE
      ${CC_OUT_PATH}
  )
endfunction()
