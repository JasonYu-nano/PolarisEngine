macro(_build_target_parse_arguments)
    set(options GAME_TARGET EDITOR_ONLY TEST_ONLY)
    set(one_value_args TARGET)
    set(multi_value_args PUBLIC_DEFINITIONS PRIVATE_DEFINITIONS PUBLIC_INCLUDE_DIRS PRIVATE_INCLUDE_DIRS PUBLIC_LINK_LIB PRIVATE_LINK_LIB DEPENDENCE_TARGETS)
    cmake_parse_arguments(ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})
endmacro()

macro(_add_compile_definitions)
    if (ARG_PUBLIC_DEFINITIONS)
        foreach (PUBLIC_DEFINITION ${ARG_PUBLIC_DEFINITIONS})
            target_compile_definitions(${ARG_TARGET} PUBLIC ${PUBLIC_DEFINITION})
        endforeach ()
    endif ()

    if (ARG_PRIVATE_DEFINITIONS)
        foreach (PRIVATE_DEFINITION ${ARG_PRIVATE_DEFINITIONS})
            target_compile_definitions(${ARG_TARGET} PRIVATE ${PRIVATE_DEFINITION})
        endforeach ()
    endif ()
endmacro()

macro(_add_include_dirs)
    if (ARG_PUBLIC_INCLUDE_DIRS)
        foreach (INCLUDE_DIR ${ARG_PUBLIC_INCLUDE_DIRS})
            target_include_directories(${ARG_TARGET} PUBLIC ${INCLUDE_DIR})
        endforeach ()
    endif ()

    if (ARG_PRIVATE_INCLUDE_DIRS)
        foreach (INCLUDE_DIR ${ARG_PRIVATE_INCLUDE_DIRS})
            target_include_directories(${ARG_TARGET} PRIVATE ${INCLUDE_DIR})
        endforeach ()
    endif ()
endmacro()

macro(_add_dependency)
    if (ARG_DEPENDENCE_TARGETS)
        foreach (TARGET ${ARG_DEPENDENCE_TARGETS})
            add_dependencies(${ARG_TARGET} ${TARGET})
        endforeach ()
    endif ()
endmacro()

macro(_add_link_libs)
    if (ARG_PUBLIC_LINK_LIB)
        foreach (LINK_LIB ${ARG_PUBLIC_LINK_LIB})
            target_link_libraries(${ARG_TARGET} PUBLIC ${LINK_LIB})
        endforeach ()
    endif ()

    if (ARG_PRIVATE_LINK_LIB)
        foreach (LINK_LIB ${ARG_PRIVATE_LINK_LIB})
            target_link_libraries(${ARG_TARGET} PRIVATE ${LINK_LIB})
        endforeach ()
    endif ()
endmacro()

macro(add_polaris_library)
    _build_target_parse_arguments(${ARGV})

    set(project_dir "${CMAKE_CURRENT_SOURCE_DIR}")

    file(GLOB_RECURSE project_files *.hpp *.cpp)

    if(shared)
        add_library(${ARG_TARGET} SHARED ${project_files})
    else()
        add_library(${ARG_TARGET} STATIC ${project_files})
    endif()

    _add_compile_definitions()
    _add_include_dirs()
    _add_link_libs()
    _add_dependency()

    target_include_directories(${ARG_TARGET} PRIVATE ${engine_generated_path}/moc/${ARG_TARGET})

    if (ARG_EDITOR_ONLY)
        set_target_properties(${ARG_TARGET} PROPERTIES EDITOR_ONLY "True")
    endif ()

    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${project_files})

    if (ARG_GAME_TARGET)
        set_target_properties(${ARG_TARGET} PROPERTIES FOLDER "Game")
    else ()
        set_target_properties(${ARG_TARGET} PROPERTIES FOLDER "Engine")
    endif ()
endmacro()

macro(add_polaris_executable)
    _build_target_parse_arguments(${ARGV})

    set(project_dir "${CMAKE_CURRENT_SOURCE_DIR}")

    file(GLOB_RECURSE project_files *.hpp *.cpp)

    add_executable(${ARG_TARGET} ${project_files})

    _add_compile_definitions()
    _add_include_dirs()
    _add_link_libs()
    _add_dependency()

    target_include_directories(${ARG_TARGET} PRIVATE ${engine_generated_path}/moc/${ARG_TARGET})

    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${project_files})

    if (ARG_GAME_TARGET)
        set_target_properties(${ARG_TARGET} PROPERTIES FOLDER "Game")
    elseif (ARG_TEST_ONLY)
        set_target_properties(${ARG_TARGET} PROPERTIES FOLDER "Test")
    else ()
        set_target_properties(${ARG_TARGET} PROPERTIES FOLDER "Engine")
    endif ()
endmacro()