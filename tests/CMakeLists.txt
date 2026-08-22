#-----------------------------------------------------------------------------------------------------------------------
# Google Test
#-----------------------------------------------------------------------------------------------------------------------
include(GoogleTest)
enable_testing()

#-----------------------------------------------------------------------------------------------------------------------
# Target: circular_array_test_deps
# Alias: collections::circular_array_test_deps
# Type: Interface Library
# Dependencies: GTest::gtest_main, GTest::gmock_main, collections::circular_array_headers
#-----------------------------------------------------------------------------------------------------------------------
# Target definition
add_library(circular_array_test_deps INTERFACE)

# Target alias
add_library(collections::circular_array_test_deps ALIAS circular_array_test_deps)

# Target dependencies
target_link_libraries(circular_array_test_deps
    INTERFACE
        collections::circular_array_headers
        GTest::gtest_main
        GTest::gmock_main
)

#-----------------------------------------------------------------------------------------------------------------------
# Target: circular_array_run_time_test
# Alias: collections::circular_array_run_time_test
# Type: Executable
# Dependencies: collections::circular_array_deps
#-----------------------------------------------------------------------------------------------------------------------
# Target definition
add_executable(circular_array_run_time_test)

# Target alias
add_executable(collections::circular_array_run_time_test ALIAS circular_array_run_time_test)

# Target sources
target_sources(circular_array_run_time_test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/circular_array_run_time_test.cpp
)

# Target dependencies
target_link_libraries(circular_array_run_time_test
    PRIVATE
        collections::circular_array_test_deps
)

# Test discovery
gtest_discover_tests(circular_array_run_time_test)

#-----------------------------------------------------------------------------------------------------------------------
# Target: circular_array_compile_time_test
# Alias: collections::circular_array_compile_time_test
# Type: Executable
# Dependencies: collections::circular_array_deps
#-----------------------------------------------------------------------------------------------------------------------
# Target definition
add_executable(circular_array_compile_time_test)

# Target alias
add_executable(collections::circular_array_compile_time_test ALIAS circular_array_compile_time_test)

# Target sources
target_sources(circular_array_compile_time_test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/circular_array_compile_time_test.cpp
)

# Target dependencies
target_link_libraries(circular_array_compile_time_test
    PRIVATE
        collections::circular_array_test_deps
)

# Test discovery
gtest_discover_tests(circular_array_compile_time_test)