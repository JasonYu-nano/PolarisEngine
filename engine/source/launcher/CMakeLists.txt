add_polaris_executable(
        TARGET launcher
        PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        PRIVATE_LINK_LIB app renderer object
)

compile_meta_object(CONTENT source)
