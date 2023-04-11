set(target renderer)

set(project_dir "${CMAKE_CURRENT_SOURCE_DIR}")

if (NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "Can't find vulkan sdk")
endif ()

file(TO_CMAKE_PATH $ENV{VULKAN_SDK} vulkan_path)

add_polaris_engine_library(
        TARGET renderer
        PRIVATE_DEFINITIONS RENDER_EXPORT
        PUBLIC_INCLUDE_DIRS ${project_dir}/include ${vulkan_path}/Include
        PRIVATE_INCLUDE_DIRS ${project_dir}/src
        PUBLIC_LINK_LIB app
        PRIVATE_LINK_LIB ${vulkan_path}/Lib/vulkan-1.lib
        DEPENDENCE_TARGETS shader_target
)