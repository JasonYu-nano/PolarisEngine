set(target_binary_path ${CMAKE_BINARY_DIR}/test/)
list(APPEND CMAKE_MODULE_PATH ${target_binary_path})

conan_cmake_autodetect(settings USE_CXX_STANDARD 17)

conan_cmake_configure(REQUIRES gtest/cci.20210126
    GENERATORS CMakeDeps CMakeToolchain
    IMPORTS "bin, *.dll -> ${CMAKE_BINARY_DIR}/output/bin"
    OPTIONS gtest/*:shared=False)

conan_cmake_install(PATH_OR_REFERENCE .
    BUILD missing
    SETTINGS ${settings}
    REMOTE conancenter)

set(GTest_DIR ${target_binary_path})

find_package(GTest CONFIG REQUIRED)

add_subdirectory(core_test)
add_subdirectory(taskflow_test)