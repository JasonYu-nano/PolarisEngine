list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/test)

conan_cmake_configure(REQUIRES gtest/cci.20210126
        GENERATORS CMakeToolchain CMakeDeps
        IMPORTS "bin, *.dll -> ${CMAKE_BINARY_DIR}/output/bin"
        OPTIONS gtest/*:shared=False)

conan_cmake_autodetect(settings
        USE_CXX_STANDARD 17)

conan_cmake_install(PATH_OR_REFERENCE .
        BUILD missing
        SETTINGS ${settings}
        REMOTE conancenter)

find_package(GTest)

add_subdirectory(core_test)
add_subdirectory(taskflow_test)
add_subdirectory(object_test)