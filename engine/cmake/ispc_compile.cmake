function(compile_ispc ISPC_FILES TARGET_NAME OWNER_TARGET GENERATED_DIR ISPC_BIN GENERATED_OBJS)

    set(working_dir "${CMAKE_CURRENT_SOURCE_DIR}")

    if(UNIX)
        execute_process(COMMAND chmod a+x ${ISPC_BIN})
    endif()

    set(OBJ_SUFFIX "")
    if (WIN32)
        set(OBJ_SUFFIX ".obj")
    endif()

    set(OUTPUT_BIN_PATH "${GENERATED_DIR}/bin")
    set(OUTPUT_INCLUDE_PATH "${GENERATED_DIR}/include/ispc")

    if (NOT EXISTS ${GENERATED_DIR})
        file(MAKE_DIRECTORY ${GENERATED_DIR})
    endif()

    if (NOT EXISTS ${OUTPUT_BIN_PATH})
        file(MAKE_DIRECTORY ${OUTPUT_BIN_PATH})
    endif()

    if (NOT EXISTS ${OUTPUT_INCLUDE_PATH})
        file(MAKE_DIRECTORY ${OUTPUT_INCLUDE_PATH})
    endif()

    set(ALL_GENERATED_ISPC_FILES "")

    set(COMPILE_TARGET host)
    if (support_sse42 OR support_sse41)
        set(COMPILE_TARGET sse4-i32x4)
    elseif(support_sse2)
        set(COMPILE_TARGET sse2-i32x4)
    endif()

    foreach(ISPC ${ISPC_FILES})
        get_filename_component(FILE_NAME ${ISPC} NAME_WLE)

        set(OUTPUT_OBJ "${OUTPUT_BIN_PATH}/${FILE_NAME}${OBJ_SUFFIX}")
        set(ISPC_INCLUDE "${OUTPUT_INCLUDE_PATH}/${FILE_NAME}.hpp")

        add_custom_command(
            OUTPUT ${OUTPUT_OBJ}
            COMMAND ${ISPC_BIN} -o ${OUTPUT_OBJ} -h ${ISPC_INCLUDE} ${ISPC} --arch=x86-64 --target=${COMPILE_TARGET} -O1
            DEPENDS ${ISPC}
        )

        list(APPEND ALL_GENERATED_ISPC_FILES ${OUTPUT_OBJ})
    endforeach()

    add_custom_target(${TARGET_NAME} DEPENDS ${ALL_GENERATED_ISPC_FILES})
    target_include_directories(${OWNER_TARGET} PUBLIC ${GENERATED_DIR}/include)

    set(${GENERATED_OBJS} ${ALL_GENERATED_ISPC_FILES} PARENT_SCOPE)

endfunction()