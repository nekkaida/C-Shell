#include "test_framework.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Test runner for C Shell unit tests
int main(int argc, char *argv[]) {
    // Initialize default test suite
    TestSuite suite;
    test_suite_init(&suite, "Default Test Suite");
    
    // Add tests to the suite
    // extern functions defined in test implementation files
    extern void register_tests(TestSuite *suite);
    register_tests(&suite);
    
    // Run the tests
    int result = test_suite_run(&suite);
    
    // Return 0 if all tests passed, otherwise return number of failures
    return result;
}