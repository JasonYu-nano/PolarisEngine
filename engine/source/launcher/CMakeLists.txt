set(target launcher)

set(project_dir "${CMAKE_CURRENT_LIST_DIR}")

file(GLOB_RECURSE project_files *.hpp *.cpp)

add_executable(${target} ${project_files})

# compile marco
target_compile_definitions(${target} PRIVATE RENDER_EXPORT)

# include dir
target_include_directories(${target} PUBLIC ${CMAKE_SOURCE_DIR}/source/launcher)

# dependency
target_link_libraries(${target} PUBLIC app renderer)

# ide
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${project_files})
set_target_properties(${target} PROPERTIES FOLDER "Engine")