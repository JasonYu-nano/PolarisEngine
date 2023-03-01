set(target benchmark)

set(project_dir "${CMAKE_CURRENT_LIST_DIR}")

file(GLOB_RECURSE project_files *.hpp *.cpp)

add_executable(${target} ${project_files})

# pch
# target_precompile_headers(${target} PRIVATE precompiled_core.hpp)

# include dir
target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/benchmark)


# add 3rd dependency

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/benchmark)

conan_cmake_configure(REQUIRES benchmark/1.6.0
        GENERATORS CMakeToolchain CMakeDeps
        IMPORTS "bin, *.dll -> ${CMAKE_BINARY_DIR}/output/bin"
        OPTIONS benchmark/*:shared=False)

conan_cmake_autodetect(settings
        USE_CXX_STANDARD 17)

conan_cmake_install(PATH_OR_REFERENCE .
        BUILD missing
        SETTINGS ${settings}
        REMOTE conancenter)

find_package(benchmark)

if(benchmark_FOUND)
    target_include_directories(${target} PRIVATE ${benchmark_INCLUDE_DIR})
    target_link_libraries(${target} PRIVATE core ${benchmark_LIBRARIES})
else()
    message(FATAL_ERROR "Can't setup benchmark dependency")
endif()

# ide
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${project_files})
set_target_properties(${target} PROPERTIES FOLDER "Test")