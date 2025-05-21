# Tests configuration
include(CTest)

# Unit test sources
set(UNIT_TEST_SOURCES
    unit/test_parser.c
    unit/test_executor.c
    unit/test_redirection.c
    unit/test_builtins.c
    unit/test_completion.c
    unit/test_path.c
)

# Define test runner framework
add_library(test_runner STATIC
    framework/test_runner.c
    framework/test_framework.c
)
target_include_directories(test_runner PUBLIC framework)

# Define unit test executable
foreach(test_src ${UNIT_TEST_SOURCES})
    # Extract test name from file path (strip directory and extension)
    get_filename_component(test_name ${test_src} NAME_WE)
    
    # Add the test executable
    add_executable(${test_name} ${test_src})
    
    # Link with test runner and shell components
    target_link_libraries(${test_name} test_runner)
    
    # Add to CTest
    add_test(NAME ${test_name} COMMAND ${test_name})
    
    # Set test properties
    set_tests_properties(${test_name} PROPERTIES
        TIMEOUT 5  # 5 second timeout
        ENVIRONMENT "TEST_MODE=1"
    )
endforeach()

# Integration test script
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/integration/run_integration_tests.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/run_integration_tests.sh
    @ONLY
)

# Make the script executable
file(CHMOD ${CMAKE_CURRENT_BINARY_DIR}/run_integration_tests.sh
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

# Add integration tests
add_test(
    NAME integration_tests
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/run_integration_tests.sh
)

# Add test fixtures directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/fixtures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Print configuration
message(STATUS "Tests configured with ${CMAKE_CURRENT_SOURCE_DIR}/fixtures as fixtures directory")