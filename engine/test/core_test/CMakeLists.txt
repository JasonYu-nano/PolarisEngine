set(target core_test)

set(project_dir "${CMAKE_CURRENT_LIST_DIR}")

#collect user files
file(GLOB_RECURSE project_files  *.hpp *.cpp)
#collect generated files
file(GLOB_RECURSE generated_files ${PROJECT_SOURCE_DIR}/intermediate/generated/test/*.hpp ${PROJECT_SOURCE_DIR}/intermediate/generated/test/*.cpp)

list(APPEND compile_files ${project_files})
list(APPEND compile_files ${generated_files})

add_executable(${target} ${compile_files})

# pch
# target_precompile_headers(${target} PRIVATE precompiled_core.hpp)

# include dir
target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/test/core_test)
target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/intermediate/generated/test)


# add 3rd dependency

if(GTest_FOUND)
    target_include_directories(${target} PRIVATE ${GTest_INCLUDE_DIR})
    target_link_libraries(${target} PRIVATE core PRIVATE ${GTest_LIBRARIES})
else()
    message(FATAL_ERROR "Can't setup gtest dependency")
endif()

# ide
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${project_files})
set_target_properties(${target} PROPERTIES FOLDER "Test")