try_run(
        run_result
        compile_result
        "${CMAKE_BINARY_DIR}/CMakeFiles"
        "${CMAKE_SOURCE_DIR}/tools/feature_detector/feature_detector.cpp"
        RUN_OUTPUT_VARIABLE features
        CMAKE_FLAGS CMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
)

message(${features})

string(REGEX MATCH "SSE=TRUE" contain_sse_string "${features}")
if (contain_sse_string)
    set(support_sse TRUE)
endif()

string(REGEX MATCH "SSE2=TRUE" contain_sse_string "${features}")
if (contain_sse_string)
    set(support_sse2 TRUE)
endif()

string(REGEX MATCH "SSE3=TRUE" contain_sse_string "${features}")
if (contain_sse_string)
    set(support_sse3 TRUE)
endif()

string(REGEX MATCH "SSSE3=TRUE" contain_sse_string "${features}")
if (contain_sse_string)
    set(support_ssse3 TRUE)
endif()

string(REGEX MATCH "SSE41=TRUE" contain_sse_string "${features}")
if (contain_sse_string)
    set(support_sse41 TRUE)
endif()

string(REGEX MATCH "SSE42=TRUE" contain_sse_string "${features}")
if (contain_sse_string)
    set(support_sse42 TRUE)
endif()