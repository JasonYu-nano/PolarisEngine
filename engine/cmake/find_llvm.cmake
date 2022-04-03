# by default, only use the libclang static lib on MSVC
if (NOT DEFINED libclang_use_static_lib)
    if (WIN32)
        set(libclang_use_static_lib YES)
    else ()
        set(libclang_use_static_lib NO)
    endif ()
endif ()

set(llvm_search_paths ${LLVM_ROOT} $ENV{LLVM_ROOT})


set(libclang_static_lib_name
        "libclang${CMAKE_STATIC_LIBRARY_SUFFIX}"
        )

set(libclang_shared_lib_name
        "libclang${CMAKE_SHARED_LIBRARY_SUFFIX}"
        )

if (WIN32)
    set(libclang_static_lib_name "libclang.lib")
    set(libclang_shared_lib_name "libclang.dll")
endif()


# include directories
find_path(llvm_include_dirs
        NAMES "clang-c/Index.h"
        PATHS ${llvm_search_paths}
        PATH_SUFFIXES "include"
        )

if (libclang_use_static_lib)
    # find static library directory
    find_path(llvm_library_dirs
            NAMES ${libclang_static_lib_name}
            PATHS ${llvm_search_paths}
            PATH_SUFFIXES "lib" "bin"
            )
endif ()

# shared library directory
find_path(llvm_binary_dir
        NAMES ${libclang_shared_lib_name}
        PATHS ${llvm_search_paths}
        PATH_SUFFIXES "bin" "lib"
        )

# unable to find everything
if (NOT llvm_include_dirs OR
        NOT llvm_binary_dir OR
(NOT llvm_library_dirs AND libclang_use_static_lib))
    message(SEND_ERROR
            "Unable to find LLVM installation. "
            "Make sure that \"LLVM_ROOT\" is set with the installation directory in either an environment variable or through the CMake GUI."
            )
endif ()

set(libclang_shared_library
        "${llvm_binary_dir}/${libclang_shared_lib_name}"
        )

if (libclang_use_static_lib)
    set(libclang_library
            "${llvm_library_dirs}/${libclang_static_lib_name}"
            )
else ()
    # we can assume this is a unix system, in which case we link to the shared object file
    set(libclang_library ${libclang_shared_library})
endif ()