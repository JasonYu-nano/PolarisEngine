if (NOT GTest_FOUND)
    message(FATAL_ERROR "Can't setup gtest dependency")
endif ()

add_polaris_executable(
        TARGET core_test
        PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${GTest_INCLUDE_DIR}
        PUBLIC_LINK_LIB app
        PRIVATE_LINK_LIB core object ${GTest_LIBRARIES}
        TEST_TARGET
        EDITOR_ONLY
)