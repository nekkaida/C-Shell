#include "test_framework.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// Maximum number of test cases in a suite
#define MAX_TEST_CASES 100

// Initialize a test suite
void test_suite_init(TestSuite *suite, const char *name) {
    if (!suite) return;
    
    suite->name = name;
    suite->test_cases = calloc(MAX_TEST_CASES, sizeof(TestCase));
    suite->test_count = 0;
    suite->passed = 0;
    suite->failed = 0;
    suite->skipped = 0;
}

// Add a test case to the suite
void test_suite_add(TestSuite *suite, const char *name, TestFunction test_func) {
    if (!suite || !name || !test_func || suite->test_count >= MAX_TEST_CASES) return;
    
    TestCase *test_case = &suite->test_cases[suite->test_count++];
    test_case->name = name;
    test_case->test_func = test_func;
    test_case->enabled = true;
}

// Run all tests in the suite
int test_suite_run(TestSuite *suite) {
    if (!suite) return -1;
    
    printf("Running test suite: %s\n", suite->name);
    printf("========================================\n");
    
    clock_t start = clock();
    
    for (int i = 0; i < suite->test_count; i++) {
        TestCase *test_case = &suite->test_cases[i];
        
        if (!test_case->enabled) {
            printf("SKIPPED: %s\n", test_case->name);
            suite->skipped++;
            continue;
        }
        
        printf("Running: %s... ", test_case->name);
        fflush(stdout);
        
        TestResult result = test_case->test_func();
        
        switch (result) {
            case TEST_PASSED:
                printf("PASSED\n");
                suite->passed++;
                break;
                
            case TEST_FAILED:
                printf("FAILED\n");
                suite->failed++;
                break;
                
            case TEST_SKIPPED:
                printf("SKIPPED\n");
                suite->skipped++;
                break;
                
            default:
                printf("UNKNOWN RESULT\n");
                suite->failed++;
                break;
        }
    }
    
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("========================================\n");
    printf("Results for %s:\n", suite->name);
    printf("  Passed:  %d\n", suite->passed);
    printf("  Failed:  %d\n", suite->failed);
    printf("  Skipped: %d\n", suite->skipped);
    printf("  Total:   %d\n", suite->test_count);
    printf("Completed in %.3f seconds\n", elapsed);
    
    // Free allocated memory
    free(suite->test_cases);
    
    // Return 0 if all tests passed, otherwise the number of failures
    return suite->failed;
}