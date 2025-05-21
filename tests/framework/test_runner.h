#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include "test_framework.h"

/**
 * Test runner header for C Shell unit tests
 * 
 * This file provides macros for creating a test entry point
 * and registering tests with the test suite.
 */

// Define the test registration function
#define DEFINE_TEST_REGISTER(test_functions...) \
    void register_tests(TestSuite *suite) { \
        test_functions \
    }

// Register a test with the suite
#define REGISTER_TEST(suite, test_name) \
    test_suite_add(suite, #test_name, test_name)

// Helper macro to create a test entry point
#define TEST_MAIN() \
    extern void register_tests(TestSuite *suite); \
    int main(int argc, char *argv[]) { \
        TestSuite suite; \
        test_suite_init(&suite, argv[0]); \
        register_tests(&suite); \
        return test_suite_run(&suite); \
    }

#endif /* TEST_RUNNER_H */