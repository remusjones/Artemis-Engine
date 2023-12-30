set(SHADER_SOURCE_DIRECTORY "${PROJECT_SOURCE_DIR}/Shaders")

function(compile_shaders DIR)
    file(GLOB_RECURSE SHADERS ${DIR}/*.vert ${DIR}/*.frag ${DIR}/*.glsl)
    foreach(SHADER ${SHADERS})
        get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
        set(OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.spv")

        add_custom_command(
                OUTPUT ${OUTPUT_FILE}
                COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} ${SHADER} -o ${OUTPUT_FILE} -V
                MAIN_DEPENDENCY ${SHADER}
                COMMENT "Compiling ${SHADER_NAME}"
        )

        add_custom_target(${SHADER_NAME} DEPENDS ${OUTPUT_FILE})
        add_dependencies(${PROJECT_NAME} ${SHADER_NAME})
    endforeach()
endfunction()
compile_shaders("${SHADER_SOURCE_DIRECTORY}")