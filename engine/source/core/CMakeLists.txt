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

if (NOT WIN32)
    set(install_icu ON)
endif ()

if (install_icu)
    conan_cmake_configure(REQUIRES icu/71.1
        GENERATORS cmake_find_package
        IMPORTS "bin, *.dll -> ${CMAKE_BINARY_DIR}/output/bin"
        OPTIONS icu:shared=True icu:with_icuio=False)
endif()

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

if(install_icu AND icu_FOUND)
    target_include_directories(${target} PUBLIC ${icu_INCLUDE_DIR})
    target_link_libraries(${target} ${icu_LIBRARIES})
    target_compile_definitions(${target} PUBLIC ${icu_DEFINITIONS})
else()
    target_include_directories(${target} PUBLIC ${CMAKE_SOURCE_DIR}/third_parties/icu/include)

    if ("amd64" IN_LIST CMAKE_CPU_ARCHITECTURES)
        if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
            target_link_libraries(${target} ${CMAKE_SOURCE_DIR}/third_parties/icu/win/x64/lib/icuucd.lib)
            target_link_libraries(${target} ${CMAKE_SOURCE_DIR}/third_parties/icu/win/x64/lib/icuind.lib)
        else()
            target_link_libraries(${target} ${CMAKE_SOURCE_DIR}/third_parties/icu/win/x64/lib/icuuc.lib)
            target_link_libraries(${target} ${CMAKE_SOURCE_DIR}/third_parties/icu/win/x64/lib/icuin.lib)
        endif()
    endif()
    #TODO:
endif()

# ide
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${project_files})
set_target_properties(${target} PROPERTIES FOLDER "Engine")

if(MSVC)
    target_compile_options(${target} PUBLIC /wd4251 /wd4624)
endif()