set(ISPC_SEARCH_PATHS $ENV{ISPC_ROOT})

set(SEARCH_ISPC_NAME)
if (WIN32)
    set(SEARCH_ISPC_NAME "bin/ispc.exe")
else ()
    message(SEND_ERROR "Un implement find ispc platform")
endif ()

find_path(ISPC_PATH_PREFIX
    NAMES ${SEARCH_ISPC_NAME}
    PATHS ${ISPC_SEARCH_PATHS}
    )

if (NOT ISPC_PATH_PREFIX)
    message(SEND_ERROR
        "Unable to find ispc execution. "
        "Make sure that \"ISPC_ROOT\" is set with the installation directory in either an environment variable or through the CMake GUI.")
else()
    set(ISPC_EXECUTION_PATH ${ISPC_PATH_PREFIX}/${SEARCH_ISPC_NAME} CACHE FILEPATH "ISPC_EXECUTION_PATH")
    message(${ISPC_EXECUTION_PATH})
endif ()