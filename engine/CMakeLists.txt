project(PolarisEngine LANGUAGES CXX)

cmake_minimum_required(VERSION 3.18.0)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# compile option
option(shared "build with dynamic link" ON)
option(unicode "build with unicode string" ON)
option(with_test "build with unit test" ON)
option(with_benchmark "build with benchmark" ON)

if(shared)
    add_compile_definitions(PL_SHARED)
endif()

if(unicode)
    add_compile_definitions(UNICODE)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    add_compile_definitions(DEBUG)
elseif(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
    add_compile_definitions(DEVELOPMENT)
elseif(${CMAKE_BUILD_TYPE} MATCHES "Release")
    add_compile_definitions("SHIPPING")
endif()
#add_compile_definitions(SPDLOG_SHARED_LIB)

if(WIN32)
    add_compile_definitions(WIN32)
    add_compile_definitions(_WIN32)
endif()

# add cmake find path

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

# set output
# TODO: use different dir for config
set(CMAKE_DEBUG_POSTFIX "d")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin)

#add sub dir

set(solution_dir "${CMAKE_CURRENT_LIST_DIR}")

include(${CMAKE_SOURCE_DIR}/cmake/conan.cmake)

add_subdirectory(source)

if(with_test)
    add_subdirectory(test)
endif()

if(with_benchmark)
    add_subdirectory(benchmark)
endif()