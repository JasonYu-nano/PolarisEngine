set(target taskflow_test)

set(project_dir "${CMAKE_CURRENT_LIST_DIR}")

file(GLOB_RECURSE project_files *.hpp *.cpp *.natvis)

add_executable(${target} ${project_files})

# pch
# target_precompile_headers(${target} PRIVATE precompiled_core.hpp)

# include dir
target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/test/taskflow_test)


# add 3rd dependency

if(GTest_FOUND)
    target_include_directories(${target} PRIVATE ${GTest_INCLUDE_DIR})
    target_link_libraries(${target} PRIVATE taskflow PRIVATE ${GTest_LIBRARIES})
else()
    message(FATAL_ERROR "Can't setup gtest dependency")
endif()

# ide
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${project_files})
set_target_properties(${target} PROPERTIES FOLDER "Test")