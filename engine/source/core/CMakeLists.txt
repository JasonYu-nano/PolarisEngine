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

if(NOT fmt_FOUND)
    message(FATAL_ERROR Can't find fmt lib)
endif()

if(NOT spdlog_FOUND)
    message(FATAL_ERROR Can't find spdlog lib)
endif()

if(NOT cxxopts_FOUND)
    message(FATAL_ERROR Can't find cxxopts lib)
endif()

add_polaris_library(
        TARGET core
        PUBLIC_DEFINITIONS ${spdlog_DEFINITIONS} ${cxxopts_DEFINITIONS}
        PRIVATE_DEFINITIONS CORE_EXPORT
        PUBLIC_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include/global ${spdlog_INCLUDE_DIR} ${cxxopts_INCLUDE_DIR}
        PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src ${fmt_INCLUDE_DIR}
        PUBLIC_LINK_LIB ${spdlog_LIBRARIES} ${cxxopts_LIBRARIES}
        PRIVATE_LINK_LIB ${fmt_LIBRARY}
)

conan_imports(core "spdlog")

add_3rd_dependency(core "icu")

if(WIN32)
    if(unicode)
        target_compile_definitions(core PUBLIC ENGINE_ROOT_PATH="${CMAKE_SOURCE_DIR}")
    elseif()
        target_compile_definitions(core PUBLIC ENGINE_ROOT_PATH="${CMAKE_SOURCE_DIR}")
    endif()
elseif()
    target_compile_definitions(core PUBLIC ENGINE_ROOT_PATH="${CMAKE_SOURCE_DIR}")
endif()

if(MSVC)
    target_compile_options(core PUBLIC /wd4251 /wd4624)
endif()

if (use_ispc)
    include(${CMAKE_SOURCE_DIR}/cmake/ispc_compile.cmake)

    file(GLOB_RECURSE ispc_files *.ispc)

    set(ispc_output ${engine_generated_path}/core/ispc)
    set(ispc_target core_ispc)
    set(generated_objs "")
    compile_ispc("${ispc_files}" ${ispc_target} core ${ispc_output} ${ISPC_EXECUTION_PATH} generated_objs)
    add_dependencies(core ${ispc_target})
    target_link_libraries(core PRIVATE ${generated_objs})

    target_compile_definitions(core PUBLIC WITH_ISPC=1)
endif()

if (support_sse)
    target_compile_definitions(core PUBLIC SUPPORT_SSE=1)
    target_compile_definitions(core PUBLIC SSE_LEVEL=1)
endif()

if (support_sse2)
    target_compile_definitions(core PUBLIC SUPPORT_SSE2=1)
    target_compile_definitions(core PUBLIC SSE_LEVEL=2)
endif()

if (support_sse3)
    target_compile_definitions(core PUBLIC SUPPORT_SSE3=1)
    target_compile_definitions(core PUBLIC SSE_LEVEL=3)
endif()

if (support_ssse3)
    target_compile_definitions(core PUBLIC SUPPORT_SSSE3=1)
    target_compile_definitions(core PUBLIC SSE_LEVEL=4)
endif()

if (support_sse41)
    target_compile_definitions(core PUBLIC SUPPORT_SSE41=1)
    target_compile_definitions(core PUBLIC SSE_LEVEL=5)
endif()

if (support_sse42)
    target_compile_definitions(core PUBLIC SUPPORT_SSE42=1)
    target_compile_definitions(core PUBLIC SSE_LEVEL=6)
endif()