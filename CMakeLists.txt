cmake_minimum_required(VERSION 3.0.0)
project(Sort-Merge-Join CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Copy compile_commands.json to project root after generation
add_custom_target(copy-compile-commands ALL
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  "${CMAKE_BINARY_DIR}/compile_commands.json"
  "${CMAKE_SOURCE_DIR}/compile_commands.json"
)

# Add external dependencies (e.g., GoogleTest)
add_subdirectory(external)

# Include directories
include_directories(src)

# List of source files for the main executable
set(SRC_LIB_FILES
    src/disk_manager.cpp
    src/join_operation.cpp
    src/table.cpp
    src/buffer_manager.cpp
    src/parser.cpp
)

# Create the main executable
add_executable(${PROJECT_NAME} ${SRC_LIB_FILES} src/main.cpp)

# Enable testing
include(CTest)

if (BUILD_TESTING)
    enable_testing()

    add_executable(test_sort_merge_join
        tests/test_sort_merge_join.cpp
        tests/test_aux.cpp
        ${SRC_LIB_FILES}
    )

    target_include_directories(test_sort_merge_join PRIVATE src)

    target_link_libraries(test_sort_merge_join PRIVATE GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(test_sort_merge_join)
endif()
