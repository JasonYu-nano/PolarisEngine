set(target ${shader_target})

file(GLOB_RECURSE shader_files *.vert *.frag)

set(output_path ${engine_generated_path}/shader)
if (NOT EXISTS ${output_path})
    file(MAKE_DIRECTORY ${output_path})
endif()

include(${CMAKE_SOURCE_DIR}/cmake/shader_compile.cmake)

set(vulkan_path $ENV{VULKAN_SDK})
set(glslang_validator "${vulkan_path}/Bin32/glslangValidator.exe")

compile_shader(
    "${shader_files}"
    "${target}"
    ""
    "${output_path}"
    "${glslang_validator}")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${shader_files})
set_target_properties(${target} PROPERTIES FOLDER "Engine")