#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * Simple test framework for C Shell unit tests
 */

// Test result codes
typedef enum {
    TEST_PASSED = 0,
    TEST_FAILED = 1,
    TEST_SKIPPED = 2
} TestResult;

// Test function signature
typedef TestResult (*TestFunction)(void);

// Test case structure
typedef struct {
    const char *name;
    TestFunction test_func;
    bool enabled;
} TestCase;

// Test suite structure
typedef struct {
    const char *name;
    TestCase *test_cases;
    int test_count;
    int passed;
    int failed;
    int skipped;
} TestSuite;

// Initialize a test suite
void test_suite_init(TestSuite *suite, const char *name);

// Add a test case to the suite
void test_suite_add(TestSuite *suite, const char *name, TestFunction test_func);

// Run all tests in the suite
int test_suite_run(TestSuite *suite);

// Assert macros
#define ASSERT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            printf("Assertion failed at %s:%d: %s is not true\n", \
                   __FILE__, __LINE__, #expr); \
            return TEST_FAILED; \
        } \
    } while (0)

#define ASSERT_FALSE(expr) \
    do { \
        if (expr) { \
            printf("Assertion failed at %s:%d: %s is not false\n", \
                   __FILE__, __LINE__, #expr); \
            return TEST_FAILED; \
        } \
    } while (0)

#define ASSERT_EQUAL(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            printf("Assertion failed at %s:%d: %s == %s\n" \
                   "  Expected: %d\n" \
                   "  Actual:   %d\n", \
                   __FILE__, __LINE__, #expected, #actual, \
                   (expected), (actual)); \
            return TEST_FAILED; \
        } \
    } while (0)

#define ASSERT_STRING_EQUAL(expected, actual) \
    do { \
        if (strcmp((expected), (actual)) != 0) { \
            printf("Assertion failed at %s:%d: %s == %s\n" \
                   "  Expected: '%s'\n" \
                   "  Actual:   '%s'\n", \
                   __FILE__, __LINE__, #expected, #actual, \
                   (expected), (actual)); \
            return TEST_FAILED; \
        } \
    } while (0)

#define ASSERT_NULL(expr) \
    do { \
        if ((expr) != NULL) { \
            printf("Assertion failed at %s:%d: %s is not NULL\n", \
                   __FILE__, __LINE__, #expr); \
            return TEST_FAILED; \
        } \
    } while (0)

#define ASSERT_NOT_NULL(expr) \
    do { \
        if ((expr) == NULL) { \
            printf("Assertion failed at %s:%d: %s is NULL\n", \
                   __FILE__, __LINE__, #expr); \
            return TEST_FAILED; \
        } \
    } while (0)

#endif /* TEST_FRAMEWORK_H */