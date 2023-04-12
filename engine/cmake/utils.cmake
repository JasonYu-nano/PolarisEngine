function(get_all_targets result dir)
    get_property(subdirs DIRECTORY "${dir}" PROPERTY SUBDIRECTORIES)
    foreach(subdir IN LISTS subdirs)
        get_all_targets(${result} "${subdir}")
    endforeach()

    get_directory_property(sub_targets DIRECTORY "${dir}" BUILDSYSTEM_TARGETS)
    set(${result} ${${result}} ${sub_targets} PARENT_SCOPE)
endfunction()

function(get_all_moc_targets result dir)
    get_property(subdirs DIRECTORY "${dir}" PROPERTY SUBDIRECTORIES)
    foreach(subdir IN LISTS subdirs)
        get_all_moc_targets(${result} "${subdir}")
    endforeach()

    get_directory_property(sub_targets DIRECTORY "${dir}" BUILDSYSTEM_TARGETS)

    set(moc_targets)
    foreach (target IN LISTS sub_targets)
        get_target_property(moc_target ${target} MOC_TARGET)
        if (moc_target STREQUAL "True")
            list(APPEND moc_targets ${target})
        endif ()
    endforeach ()

    set(${result} ${${result}} ${moc_targets} PARENT_SCOPE)
endfunction()