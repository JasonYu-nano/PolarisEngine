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
    set(one_value_args TARGET)
    set(multi_value_args CONTENT INCLUDE_DIR)
    cmake_parse_arguments(MOC "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    set(content "")
    foreach(arg ${MOC_CONTENT})
        if (content STREQUAL "")
            set(content "${arg}")
        else()
            set(content "${content},${arg}")
        endif()
    endforeach()

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

    message("Include directory: ${include_dirs_str}")

    add_custom_target(moc
        COMMAND ${CMAKE_BINARY_DIR}/output/bin/meta_object_compiler -s 20 -c ${content} -i ${include_dirs_str})

    add_dependencies(${MOC_TARGET} moc)

endfunction()