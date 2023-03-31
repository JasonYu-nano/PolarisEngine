set(target object)

set(project_dir "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE project_files *.hpp *.cpp)

if(shared)
    add_library(${target} SHARED ${project_files})
else()
    add_library(${target} STATIC ${project_files})
endif()

# compile marco
target_compile_definitions(${target} PRIVATE OBJECT_EXPORT)

# include dir
target_include_directories(${target} PUBLIC ${project_dir}/include PRIVATE ${project_dir}/src)

# dependency
target_link_libraries(${target} PUBLIC core)

# target_link_libraries(${target} PUBLIC ${link_libs})

# ide
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${project_files})
set_target_properties(${target} PROPERTIES FOLDER "Engine")