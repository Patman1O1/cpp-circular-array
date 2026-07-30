#-----------------------------------------------------------------------------------------------------------------------
# Google Test
#-----------------------------------------------------------------------------------------------------------------------
include(GoogleTest)
enable_testing()

#-----------------------------------------------------------------------------------------------------------------------
# Target: cpp_circular_array_test
# Alias: cpp_circular_array::cpp_circular_array_test
# Type: Executable
# Dependencies: GTest::gtest_main, GTest::gmock_main, cpp_circular_array::cpp_circular_array_headers
#-----------------------------------------------------------------------------------------------------------------------
# Target definition
add_executable(cpp_circular_array_test)

# Target alias
add_executable(cpp_circular_array::cpp_circular_array_test ALIAS cpp_circular_array_test)

# Target sources
target_sources(cpp_circular_array_test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp_circular_array_test.cpp
)

# Target dependencies
target_link_libraries(cpp_circular_array_test
    PRIVATE
        cpp_circular_array::cpp_circular_array_headers
        GTest::gtest_main
        GTest::gmock_main
)

# Test discovery
gtest_discover_tests(cpp_circular_array_test)