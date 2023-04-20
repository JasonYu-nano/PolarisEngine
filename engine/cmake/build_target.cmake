macro(_build_target_parse_arguments)
    set(options EDITOR_ONLY SKIP_MOC GAME_TARGET TEST_TARGET)
    set(one_value_args TARGET)
    set(multi_value_args PUBLIC_DEFINITIONS PRIVATE_DEFINITIONS PUBLIC_INCLUDE_DIRS PRIVATE_INCLUDE_DIRS PUBLIC_LINK_LIB PRIVATE_LINK_LIB DEPENDENCE_TARGETS)
    cmake_parse_arguments(ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})
endmacro()

macro(_add_target_properties)
    if (ARG_EDITOR_ONLY)
        set_target_properties(${ARG_TARGET} PROPERTIES EDITOR_ONLY 1)
    else ()
        set_target_properties(${ARG_TARGET} PROPERTIES EDITOR_ONLY 0)
    endif ()
endmacro()

macro(_add_moc_properties)
    if (NOT SKIP_MOC)
        set_target_properties(${ARG_TARGET} PROPERTIES MOC_TARGET 1)
        target_include_directories(${ARG_TARGET} PRIVATE ${engine_generated_path}/moc/${ARG_TARGET})
    else ()
        set_target_properties(${ARG_TARGET} PROPERTIES MOC_TARGET 0)
    endif ()
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

macro(_setup_ide)
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${project_files})

    if (ARG_GAME_TARGET)
        set_target_properties(${ARG_TARGET} PROPERTIES FOLDER "Game")
    elseif (ARG_TEST_TARGET)
        set_target_properties(${ARG_TARGET} PROPERTIES FOLDER "Test")
    else ()
        set_target_properties(${ARG_TARGET} PROPERTIES FOLDER "Engine")
    endif ()
endmacro()

macro(_add_link_libs)
    get_target_property(target_editor_only ${ARG_TARGET} EDITOR_ONLY)

    if (ARG_PUBLIC_LINK_LIB)
        foreach (LINK_LIB ${ARG_PUBLIC_LINK_LIB})
            if (NOT target_editor_only AND TARGET ${LINK_LIB})
                get_property(editor_only TARGET ${LINK_LIB} PROPERTY EDITOR_ONLY)
                if (editor_only)
                    message(FATAL_ERROR "Link an editor target to runtime target is not allowed")
                endif ()
            endif ()

            target_link_libraries(${ARG_TARGET} PUBLIC ${LINK_LIB})
        endforeach ()
    endif ()

    if (ARG_PRIVATE_LINK_LIB)
        foreach (LINK_LIB ${ARG_PRIVATE_LINK_LIB})
            if (NOT target_editor_only AND TARGET ${LINK_LIB})
                get_property(editor_only TARGET ${LINK_LIB} PROPERTY EDITOR_ONLY)
                if (editor_only)
                    message(FATAL_ERROR "Link an editor target to runtime target is not allowed")
                endif ()
            endif ()

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

    _add_target_properties()
    _add_compile_definitions()
    _add_include_dirs()
    _add_link_libs()
    _add_dependency()
    _add_moc_properties()

    _setup_ide()
endmacro()

macro(add_polaris_executable)
    _build_target_parse_arguments(${ARGV})

    set(project_dir "${CMAKE_CURRENT_SOURCE_DIR}")

    file(GLOB_RECURSE project_files *.hpp *.cpp)

    add_executable(${ARG_TARGET} ${project_files})

    _add_target_properties()
    _add_compile_definitions()
    _add_include_dirs()
    _add_link_libs()
    _add_dependency()
    _add_moc_properties()

    _setup_ide()
endmacro()

set(moc_generated_path ${engine_generated_path}/moc)

function(compile_meta_object)
    set(options OPTIONAL)
    set(one_value_args)
    set(multi_value_args CONTENT)
    cmake_parse_arguments(MOC "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    list(LENGTH MOC_CONTENT num_content)
    if (num_content LESS 0)
        message(FATAL_ERROR "Content can not be empty")
    endif ()

    set(target_list)
    foreach (content ${MOC_CONTENT})
        get_all_moc_targets(target_list ${CMAKE_SOURCE_DIR}/${content})
    endforeach ()

    set(arg_include_dirs "")
    foreach(target ${target_list})
        get_target_property(include_dirs ${target} INCLUDE_DIRECTORIES)

        foreach(include_dir ${include_dirs})
            if (include_dir STREQUAL "include_dirs-NOTFOUND")
            elseif (arg_include_dirs STREQUAL "")
                set(arg_include_dirs ${include_dir})
            else()
                set(arg_include_dirs "${arg_include_dirs},${include_dir}")
            endif()
        endforeach()
    endforeach()

    list(JOIN MOC_CONTENT "," arg_content)
    list(JOIN target_list "," arg_target_list)

    message("Moc target list: ${arg_target_list}")
    message("Moc content ${arg_content}")
    message("Moc include dirs ${arg_include_dirs}")

    set(moc_exist 0)
    if (WIN32)
        if (EXISTS ${CMAKE_BINARY_DIR}/output/bin/meta_object_compiler.exe)
            set(moc_exist 1)
        endif ()
    endif ()

    if (moc_exist)
        execute_process(
                COMMAND meta_object_compiler -s 20 -c ${arg_content} -i ${arg_include_dirs} -t ${arg_target_list} -d 0 --incremental 0
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin
                RESULT_VARIABLE exit_code
        )
    endif ()

    if (exit_code)
        message("Compile meta object failed, error code: ${exit_code}")
    endif ()

    foreach(target ${target_list})
        set(target_moc_generated_path ${moc_generated_path}/${target})
        FILE(GLOB auto_generated_files "${target_moc_generated_path}/*.hpp" "${target_moc_generated_path}/*.cpp")

#        if (EXISTS ${target_moc_generated_path}/manifest.cmake)
#            include(${target_moc_generated_path}/manifest.cmake)
#            FIXME: Re-run cmake when file modified
#            get_target_property(location ${target} SOURCE_DIR)
#            set_property(DIRECTORY ${location} PROPERTY CMAKE_CONFIGURE_DEPENDS
#                    ${${target}_manifest}
#            )
#        endif()

        if (EXISTS ${target_moc_generated_path}/manifest.moc)
            file(READ ${target_moc_generated_path}/manifest.moc manifest)
            string(REPLACE "\n" ";" dependence ${manifest})
            message(${dependence})
            get_target_property(location ${target} SOURCE_DIR)
            set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY CMAKE_CONFIGURE_DEPENDS ${dependence})
        endif ()

        target_sources(${target} PRIVATE ${auto_generated_files})
        target_include_directories(${target} PUBLIC ${target_moc_generated_path})
    endforeach()

endfunction()