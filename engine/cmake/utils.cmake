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
        if (moc_target)
            list(APPEND moc_targets ${target})
        endif ()
    endforeach ()

    set(${result} ${${result}} ${moc_targets} PARENT_SCOPE)
endfunction()


function(add_3rd_dependency target package_name is_private)
    set(platform_folder "")
    if (WIN32)
        set(platform_folder "win")
    endif ()

    set(package_root ${CMAKE_SOURCE_DIR}/third_parties/${package_name}/)

    set(package_include_dir ${package_root}/include)
    if (NOT EXISTS ${package_include_dir})
        message(FATAL_ERROR "Can't find package include dir by ${package_include_dir}")
    endif()

    if (${is_private})
        target_include_directories(${target} PRIVATE ${package_include_dir})
    else ()
        target_include_directories(${target} PUBLIC ${package_include_dir})
    endif ()

    set (platform_package_root ${CMAKE_SOURCE_DIR}/third_parties/${package_name}/${platform_folder})

    if (NOT EXISTS ${platform_package_root})
        return()
    endif ()

    set(package_link_root ${platform_package_root})
    if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
        if (EXISTS ${package_link_root}/debug)
            set (package_link_root ${package_link_root}/debug)
        endif()
    else ()
        if (EXISTS ${package_link_root}/release)
            set(package_link_root ${package_link_root}/release)
        endif()
    endif ()

    set(package_link_dir ${package_link_root})

    if ("amd64" IN_LIST CMAKE_CPU_ARCHITECTURES)
        if (EXISTS ${package_link_dir}/x64)
            set(package_link_dir ${package_link_dir}/x64)
        endif()
    else()
        if (EXISTS ${package_link_dir}/x64)
            set(package_link_dir ${package_link_dir}/x86)
        endif()
    endif()

    file(GLOB link_libs ${package_link_dir}/lib/*.lib)
    file(GLOB link_dlls ${package_link_dir}/bin/*.dll)

    foreach(lib ${link_libs})
        message(STATUS "Add dependency lib: ${lib}")
    endforeach()

    if (${is_private})
        target_link_libraries(${target} PRIVATE ${link_libs})
    else ()
        target_link_libraries(${target} PUBLIC ${link_libs})
    endif ()

    foreach(dll ${link_dlls})
        file(COPY ${dll} DESTINATION ${CMAKE_BINARY_DIR}/output/bin)
        message(STATUS "Copy dependency dll: ${dll}")
    endforeach()

endfunction()

function(conan_imports target package_name)
    set(include_dir ${${package_name}_INCLUDE_DIR})
    if (include_dir)
        set(package_bin ${include_dir}/../bin/*${CMAKE_SHARED_LIBRARY_SUFFIX})
        file(GLOB dlls ${package_bin})

        foreach(dll ${dlls})
            file(COPY ${dll} DESTINATION ${CMAKE_BINARY_DIR}/output/bin)
            message(STATUS "Copy dependency dll: ${dll}")
        endforeach()
    endif()
endfunction()