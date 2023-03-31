set(target meta_object_compiler)

set(project_dir "${CMAKE_CURRENT_LIST_DIR}")

file(GLOB_RECURSE project_files *.hpp *.cpp)

add_executable(${target} ${project_files})

include(${CMAKE_SOURCE_DIR}/cmake/find_llvm.cmake)

# include dir
target_include_directories(${target} PRIVATE ${project_dir} ${project_dir}/include/global PRIVATE ${project_dir}/src ${llvm_include_dirs})

# dependency
target_link_libraries(${target} PRIVATE core ${libclang_library})
# ide
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${project_files})
set_target_properties(${target} PROPERTIES FOLDER "Engine")