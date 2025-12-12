cmake_minimum_required(VERSION 3.14)
project(EncodingTests LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the test executable
add_executable(encoding_test
    "${CMAKE_CURRENT_SOURCE_DIR}/encoding_test.cpp"
)

# Include directories - the encoding.h is in the parent's include subdirectory
target_include_directories(encoding_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

# Enable console output on Windows
if(WIN32)
    set_target_properties(encoding_test PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()

# Set output directory
set_target_properties(encoding_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
