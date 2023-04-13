add_polaris_executable(
        TARGET meta_object_compiler
        PUBLIC_INCLUDE_DIRS
        PRIVATE_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}"
        PRIVATE_LINK_LIB core
        EDITOR_ONLY
        SKIP_MOC
)

add_3rd_dependency(meta_object_compiler "lib_clang" TRUE)