list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/test)

conan_cmake_configure(REQUIRES gtest/cci.20210126
        GENERATORS cmake_find_package
        OPTIONS gtest:shared=False)

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
        BUILD missing
        SETTINGS ${settings}
        REMOTE conan-center)

find_package(GTest)

add_subdirectory(core_test)
add_subdirectory(taskflow_test)