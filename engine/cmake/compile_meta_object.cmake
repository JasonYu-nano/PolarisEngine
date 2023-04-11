function(get_all_targets result dir)
    get_property(_subdirs DIRECTORY "${dir}" PROPERTY SUBDIRECTORIES)
    foreach(_subdir IN LISTS _subdirs)
        get_all_targets(${result} "${_subdir}")
    endforeach()

    get_directory_property(_sub_targets DIRECTORY "${dir}" BUILDSYSTEM_TARGETS)
    set(${result} ${${result}} ${_sub_targets} PARENT_SCOPE)
endfunction()

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
        get_all_targets(target_list ${CMAKE_SOURCE_DIR}/${content})
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

    message(${arg_target_list})
    message(${arg_content})
    message(${arg_include_dirs})

    execute_process(
        COMMAND meta_object_compiler -s 20 -c ${arg_content} -i ${arg_include_dirs} -t ${arg_target_list}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin
    )

    foreach(target ${target_list})
        set(target_moc_generated_path ${moc_generated_path}/${target})
        FILE(GLOB auto_generated_files "${target_moc_generated_path}/*.hpp" "${target_moc_generated_path}/*.cpp")

        if (EXISTS ${target_moc_generated_path}/manifest.cmake)
            include(${target_moc_generated_path}/manifest.cmake)

            set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
                ${${target}_manifest}
            )
        endif()

        target_sources(${target} PRIVATE ${auto_generated_files})
        target_include_directories(${target} PRIVATE ${target_moc_generated_path})
    endforeach()

endfunction()

set(moc_generated_path ${engine_generated_path}/moc)

macro(compile_meta_object_v2)
    set(options OPTIONAL)
    set(one_value_args TARGET CONTENT)
    set(multi_value_args)
    cmake_parse_arguments(MOC "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if (${MOC_TARGET} STREQUAL "")
        message(FATAL_ERROR "Target can not be empty")
    endif ()

    set(include_dirs_str "")
    get_all_targets(target_List ${CMAKE_SOURCE_DIR})
    foreach(target ${target_List})
        get_target_property(include_dirs ${target} INCLUDE_DIRECTORIES)

        foreach(include_dir ${include_dirs})
            if (include_dir STREQUAL "include_dirs-NOTFOUND")
            elseif (include_dirs_str STREQUAL "")
                set(include_dirs_str ${include_dir})
            else()
                set(include_dirs_str "${include_dirs_str},${include_dir}")
            endif()
        endforeach()
    endforeach()

    set(target_moc_generated_path ${moc_generated_path}/${MOC_TARGET})

    execute_process(
        COMMAND meta_object_compiler -s 20 -c ${MOC_CONTENT} -i ${include_dirs_str} -t ${MOC_TARGET}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin
    )

    FILE(GLOB auto_generated_files "${target_moc_generated_path}/*.hpp" "${target_moc_generated_path}/*.cpp")

    if (EXISTS ${target_moc_generated_path}/manifest.cmake)
        include(${target_moc_generated_path}/manifest.cmake)

        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
            ${${MOC_TARGET}_manifest}
        )
    endif()

    target_sources(${MOC_TARGET} PRIVATE ${auto_generated_files})

endmacro()