# Test configuration for KISS Fuzzer
cmake_minimum_required(VERSION 3.13)

# Enable testing
enable_testing()

# Unity test framework
add_subdirectory(unity)

# Test utilities
add_library(test_utils
    mocks/mock_hardware.c
    mocks/mock_hardware.h
)

target_include_directories(test_utils PUBLIC
    mocks
    ../src
    unity/src
)

# Function to create a test executable
function(add_unit_test test_name source_file)
    add_executable(${test_name} ${source_file})
    
    target_link_libraries(${test_name}
        test_utils
        unity
    )
    
    target_include_directories(${test_name} PRIVATE
        unity/src
        mocks
        ../src
    )
    
    target_compile_definitions(${test_name} PRIVATE
        PICO_TESTING=1
        UNITY_INCLUDE_CONFIG_H=1
    )
    
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# Unit tests
add_unit_test(test_display unit/test_display.c)
add_unit_test(test_ui unit/test_ui.c)
add_unit_test(test_jtag unit/test_jtag.c)
add_unit_test(test_power unit/test_power.c)
add_unit_test(test_storage unit/test_storage.c)

# Integration tests
add_subdirectory(integration)

# Test coverage (if gcov available)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(test_utils PRIVATE --coverage)
    target_link_options(test_utils PRIVATE --coverage)
endif()

# Custom test target that runs all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_display test_ui test_jtag test_power test_storage
    COMMENT "Running all unit tests"
)
