set(target renderer)

set(project_dir "${CMAKE_CURRENT_LIST_DIR}")

file(GLOB_RECURSE project_files *.hpp *.cpp)

if(shared)
    add_library(${target} SHARED ${project_files})
else()
    add_library(${target} STATIC ${project_files})
endif()

# compile marco
target_compile_definitions(${target} PRIVATE RENDER_EXPORT)

# include dir
target_include_directories(${target} PUBLIC ${CMAKE_SOURCE_DIR}/source/render)

# dependency

# set(link_libs "app")

if(DEFINED ENV{VULKAN_SDK})
    set(vulkan_path $ENV{VULKAN_SDK})
    target_include_directories(${target} PUBLIC ${vulkan_path}/Include)
    #target_link_libraries(${target} ${vulkan_path}/Lib/vulkan-1)
    set(link_libs "${link_libs} ${vulkan_path}/Lib/vulkan-1")
    target_link_libraries(${target} PUBLIC app PRIVATE ${vulkan_path}/Lib/vulkan-1.lib)
else()
    message(FATAL_ERROR "Can't find vulkan sdk")
endif()

# target_link_libraries(${target} PUBLIC ${link_libs})

# ide
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${project_files})
set_target_properties(${target} PROPERTIES FOLDER "Engine")