set(target core)

set(project_dir "${CMAKE_CURRENT_LIST_DIR}")

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
        target_compile_definitions(${target} PUBLIC ENGINE_ROOT_PATH=L"${CMAKE_SOURCE_DIR}")
    elseif()
        target_compile_definitions(${target} PUBLIC ENGINE_ROOT_PATH="${CMAKE_SOURCE_DIR}")
    endif()
elseif()
    target_compile_definitions(${target} PUBLIC ENGINE_ROOT_PATH="${CMAKE_SOURCE_DIR}")
endif()

# pch
# target_precompile_headers(${target} PRIVATE precompiled_core.hpp)

# include dir
target_include_directories(${target} PUBLIC ${CMAKE_SOURCE_DIR}/source/core)


# add 3rd dependency

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/source/core)

conan_cmake_configure(REQUIRES spdlog/1.8.2
    GENERATORS cmake_find_package
    IMPORTS "bin, *.dll -> ${CMAKE_BINARY_DIR}/output/bin"
    OPTIONS spdlog:shared=True spdlog:wchar_support=True)

conan_cmake_autodetect(settings)
message(${settings})
conan_cmake_install(PATH_OR_REFERENCE .
    BUILD missing
    SETTINGS ${settings}
    REMOTE conan-center)

find_package(fmt)
find_package(spdlog)

if (install_icu)
    find_package(icu)
endif()

if(fmt_FOUND)
    target_include_directories(${target} PRIVATE ${fmt_INCLUDE_DIR})
    target_link_libraries(${target} ${fmt_LIBRARY})
endif()

if(spdlog_FOUND)
    target_include_directories(${target} PUBLIC ${spdlog_INCLUDE_DIR})
    target_link_libraries(${target} ${spdlog_LIBRARIES})
    target_compile_definitions(${target} PUBLIC ${spdlog_DEFINITIONS})
endif()

add_3rd_dependency(${target} "icu")

# ide
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${project_files})
set_target_properties(${target} PROPERTIES FOLDER "Engine")

if(MSVC)
    target_compile_options(${target} PUBLIC /wd4251 /wd4624)
endif()