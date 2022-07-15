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

if(WIN32)
    add_compile_definitions(WIN32)
    add_compile_definitions(_WIN32)
endif()

cmake_host_system_information(RESULT support_sse QUERY HAS_SSE)
cmake_host_system_information(RESULT support_sse2 QUERY HAS_SSE2)

if (support_sse)
    add_compile_definitions(SSE)
endif()

if (support_sse2)
    add_compile_definitions(SSE2)
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

include(${CMAKE_SOURCE_DIR}/cmake/conan.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/detect_cpu_architectures.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/add_dependency.cmake)

add_subdirectory(source)

if(with_test)
    add_subdirectory(test)
endif()

if(with_benchmark)
    add_subdirectory(benchmark)
endif()