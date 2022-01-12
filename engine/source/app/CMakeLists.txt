set(target app)

set(project_dir "${CMAKE_CURRENT_LIST_DIR}")

file(GLOB_RECURSE project_files *.hpp *.cpp)

if(shared)
    add_library(${target} SHARED ${project_files})
else()
    add_library(${target} STATIC ${project_files})
endif()

# compile marco
target_compile_definitions(${target} PRIVATE APP_EXPORT)

# include dir
target_include_directories(${target} PUBLIC ${CMAKE_SOURCE_DIR}/source/app)

# dependency
target_link_libraries(${target} PUBLIC core)

# ide
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${project_files})
set_target_properties(${target} PROPERTIES FOLDER "Engine")