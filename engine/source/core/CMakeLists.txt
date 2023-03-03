set(target core)

set(project_dir "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE project_files *.hpp *.cpp)

if(shared)
    add_library(${target} SHARED ${project_files})
else()
    add_library(${target} STATIC ${project_files})
endif()

# compile marco
target_compile_definitions(${target} PRIVATE CORE_EXPORT)

if(WIN32)
    if(unicode)
        target_compile_definitions(${target} PUBLIC ENGINE_ROOT_PATH="${CMAKE_SOURCE_DIR}")
    elseif()
        target_compile_definitions(${target} PUBLIC ENGINE_ROOT_PATH="${CMAKE_SOURCE_DIR}")
    endif()
elseif()
    target_compile_definitions(${target} PUBLIC ENGINE_ROOT_PATH="${CMAKE_SOURCE_DIR}")
endif()

# pch
# target_precompile_headers(${target} PRIVATE precompiled_core.hpp)

# include dir
target_include_directories(${target} PUBLIC ${project_dir}/include ${project_dir}/include/global PRIVATE ${project_dir}/src)


# add 3rd dependency

set(target_binary_path ${CMAKE_BINARY_DIR}/source/core)
list(APPEND CMAKE_MODULE_PATH ${target_binary_path})

conan_cmake_autodetect(settings USE_CXX_STANDARD 17)

conan_cmake_configure(REQUIRES spdlog/1.11.0 cxxopts/3.0.0
    GENERATORS CMakeDeps CMakeToolchain
    IMPORTS "bin, *.dll -> ${CMAKE_BINARY_DIR}/output/bin"
    OPTIONS spdlog/*:shared=True)

conan_cmake_install(PATH_OR_REFERENCE .
    BUILD missing
    SETTINGS ${settings}
    REMOTE conancenter)

set(fmt_DIR ${target_binary_path})
set(spdlog_DIR ${target_binary_path})
set(cxxopts_DIR ${target_binary_path})

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)

if(fmt_FOUND)
    target_include_directories(${target} PRIVATE ${fmt_INCLUDE_DIR})
    target_link_libraries(${target} ${fmt_LIBRARY})
endif()

if(spdlog_FOUND)
    target_include_directories(${target} PUBLIC ${spdlog_INCLUDE_DIR})
    target_link_libraries(${target} ${spdlog_LIBRARIES})
    target_compile_definitions(${target} PUBLIC ${spdlog_DEFINITIONS})
endif()

if(cxxopts_FOUND)
    target_include_directories(${target} PUBLIC ${cxxopts_INCLUDE_DIR})
    target_link_libraries(${target} ${cxxopts_LIBRARIES})
    target_compile_definitions(${target} PUBLIC ${cxxopts_DEFINITIONS})
endif()

add_3rd_dependency(${target} "icu")

# ide
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${project_files})
set_target_properties(${target} PROPERTIES FOLDER "Engine")

if(MSVC)
    target_compile_options(${target} PUBLIC /wd4251 /wd4624)
endif()

if (use_ispc)
    include(${CMAKE_SOURCE_DIR}/cmake/ispc_compile.cmake)

    file(GLOB_RECURSE ispc_files *.ispc)

    set(ispc_output ${engine_generated_path}/core/ispc)
    set(ispc_target core_ispc)
    set(generated_objs "")
    compile_ispc("${ispc_files}" ${ispc_target} ${target} ${ispc_output} ${ISPC_EXECUTION_PATH} generated_objs)
    add_dependencies(${target} ${ispc_target})
    target_link_libraries(${target} ${generated_objs})

    target_compile_definitions(${target} PUBLIC WITH_ISPC=1)
endif()

if (support_sse)
    target_compile_definitions(${target} PUBLIC SUPPORT_SSE=1)
    target_compile_definitions(${target} PUBLIC SSE_LEVEL=1)
endif()

if (support_sse2)
    target_compile_definitions(${target} PUBLIC SUPPORT_SSE2=1)
    target_compile_definitions(${target} PUBLIC SSE_LEVEL=2)
endif()

if (support_sse3)
    target_compile_definitions(${target} PUBLIC SUPPORT_SSE3=1)
    target_compile_definitions(${target} PUBLIC SSE_LEVEL=3)
endif()

if (support_ssse3)
    target_compile_definitions(${target} PUBLIC SUPPORT_SSSE3=1)
    target_compile_definitions(${target} PUBLIC SSE_LEVEL=4)
endif()

if (support_sse41)
    target_compile_definitions(${target} PUBLIC SUPPORT_SSE41=1)
    target_compile_definitions(${target} PUBLIC SSE_LEVEL=5)
endif()

if (support_sse42)
    target_compile_definitions(${target} PUBLIC SUPPORT_SSE42=1)
    target_compile_definitions(${target} PUBLIC SSE_LEVEL=6)
endif()