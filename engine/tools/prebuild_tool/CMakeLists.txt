set(target prebuild_tool)

set(project_dir "${CMAKE_CURRENT_LIST_DIR}")

file(GLOB_RECURSE project_files *.hpp *.cpp)

add_executable(${target} ${project_files})

include(find_llvm)
# pch

# include dir
target_include_directories(${target}
        PRIVATE ${CMAKE_SOURCE_DIR}/tools/prebuild_tool
        PRIVATE ${llvm_include_dirs})

# add 3rd dependency
target_link_libraries(${target} PUBLIC core PRIVATE ${libclang_library})

target_link_directories(${target} PUBLIC ${CONAN_LIB_DIRS_DEBUG} PUBLIC ${CONAN_LIB_DIRS_RELEASE})

# ide
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${project_files})
set_target_properties(${target} PROPERTIES FOLDER "Tools")