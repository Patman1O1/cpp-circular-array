#-----------------------------------------------------------------------------------------------------------------------
# Google Test
#-----------------------------------------------------------------------------------------------------------------------
include(GoogleTest)
enable_testing()

#-----------------------------------------------------------------------------------------------------------------------
# Target: circular_array_test
# Alias: collections::circular_array_test
# Type: Executable
# Dependencies: GTest::gtest_main, GTest::gmock_main, collections::circular_array_headers
#-----------------------------------------------------------------------------------------------------------------------
# Target definition
add_executable(circular_array_test)

# Target alias
add_executable(collections::circular_array_test ALIAS circular_array_test)

# Target sources
target_sources(circular_array_test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/circular_array_test.cpp
)

# Target dependencies
target_link_libraries(circular_array_test
    PRIVATE
        collections::circular_array_headers
        GTest::gtest_main
        GTest::gmock_main
)

# Test discovery
gtest_discover_tests(circular_array_test)