/**
 * @file test_utils.c
 * @brief Unit tests for utility functions
 */

#include "../../include/utils/utils.h"
#include "../../include/utils/string.h"
#include "../../include/utils/path.h"
#include "../../include/utils/memory.h"
#include "../framework/test_framework.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Test safe_strcpy function
 */
TestResult test_safe_strcpy() {
    char dest[10];
    
    // Test normal case
    safe_strcpy(dest, "hello", sizeof(dest));
    ASSERT_STRING_EQUAL("hello", dest);
    
    // Test truncation
    safe_strcpy(dest, "hello world", sizeof(dest));
    ASSERT_STRING_EQUAL("hello wor", dest);
    
    // Test empty string
    safe_strcpy(dest, "", sizeof(dest));
    ASSERT_STRING_EQUAL("", dest);
    
    return TEST_PASSED;
}

/**
 * @brief Test safe_strcat function
 */
TestResult test_safe_strcat() {
    char dest[10];
    
    // Test normal case
    safe_strcpy(dest, "hello", sizeof(dest));
    safe_strcat(dest, " you", sizeof(dest));
    ASSERT_STRING_EQUAL("hello you", dest);
    
    // Test truncation
    safe_strcpy(dest, "hello", sizeof(dest));
    safe_strcat(dest, " world", sizeof(dest));
    ASSERT_STRING_EQUAL("hello wor", dest);
    
    // Test empty string
    safe_strcpy(dest, "hello", sizeof(dest));
    safe_strcat(dest, "", sizeof(dest));
    ASSERT_STRING_EQUAL("hello", dest);
    
    return TEST_PASSED;
}

/**
 * @brief Test str_trim function
 */
TestResult test_str_trim() {
    char str1[] = "  hello  ";
    char str2[] = "hello";
    char str3[] = "  ";
    char str4[] = "";
    
    // Test with leading and trailing spaces
    str_trim(str1);
    ASSERT_STRING_EQUAL("hello", str1);
    
    // Test with no spaces
    str_trim(str2);
    ASSERT_STRING_EQUAL("hello", str2);
    
    // Test with only spaces
    str_trim(str3);
    ASSERT_STRING_EQUAL("", str3);
    
    // Test with empty string
    str_trim(str4);
    ASSERT_STRING_EQUAL("", str4);
    
    return TEST_PASSED;
}

/**
 * @brief Test str_split function
 */
TestResult test_str_split() {
    int count;
    char **parts;
    
    // Test normal case
    parts = str_split("hello,world,test", ',', &count);
    ASSERT_NOT_NULL(parts);
    ASSERT_EQUAL(3, count);
    ASSERT_STRING_EQUAL("hello", parts[0]);
    ASSERT_STRING_EQUAL("world", parts[1]);
    ASSERT_STRING_EQUAL("test", parts[2]);
    free_str_array(parts, count);
    
    // Test with no delimiter
    parts = str_split("hello", ',', &count);
    ASSERT_NOT_NULL(parts);
    ASSERT_EQUAL(1, count);
    ASSERT_STRING_EQUAL("hello", parts[0]);
    free_str_array(parts, count);
    
    // Test with empty string
    parts = str_split("", ',', &count);
    ASSERT_NOT_NULL(parts);
    ASSERT_EQUAL(1, count);
    ASSERT_STRING_EQUAL("", parts[0]);
    free_str_array(parts, count);
    
    // Test with delimiter at the beginning
    parts = str_split(",hello,world", ',', &count);
    ASSERT_NOT_NULL(parts);
    ASSERT_EQUAL(3, count);
    ASSERT_STRING_EQUAL("", parts[0]);
    ASSERT_STRING_EQUAL("hello", parts[1]);
    ASSERT_STRING_EQUAL("world", parts[2]);
    free_str_array(parts, count);
    
    // Test with delimiter at the end
    parts = str_split("hello,world,", ',', &count);
    ASSERT_NOT_NULL(parts);
    ASSERT_EQUAL(3, count);
    ASSERT_STRING_EQUAL("hello", parts[0]);
    ASSERT_STRING_EQUAL("world", parts[1]);
    ASSERT_STRING_EQUAL("", parts[2]);
    free_str_array(parts, count);
    
    return TEST_PASSED;
}

/**
 * @brief Test str_starts_with function
 */
TestResult test_str_starts_with() {
    // Test matching prefix
    ASSERT_TRUE(str_starts_with("hello world", "hello"));
    
    // Test non-matching prefix
    ASSERT_FALSE(str_starts_with("hello world", "world"));
    
    // Test with empty string
    ASSERT_TRUE(str_starts_with("hello", ""));
    ASSERT_FALSE(str_starts_with("", "hello"));
    
    // Test with NULL
    ASSERT_FALSE(str_starts_with(NULL, "hello"));
    ASSERT_FALSE(str_starts_with("hello", NULL));
    
    return TEST_PASSED;
}

/**
 * @brief Test str_ends_with function
 */
TestResult test_str_ends_with() {
    // Test matching suffix
    ASSERT_TRUE(str_ends_with("hello world", "world"));
    
    // Test non-matching suffix
    ASSERT_FALSE(str_ends_with("hello world", "hello"));
    
    // Test with empty string
    ASSERT_TRUE(str_ends_with("hello", ""));
    ASSERT_FALSE(str_ends_with("", "hello"));
    
    // Test with NULL
    ASSERT_FALSE(str_ends_with(NULL, "hello"));
    ASSERT_FALSE(str_ends_with("hello", NULL));
    
    return TEST_PASSED;
}

/**
 * @brief Test str_replace function
 */
TestResult test_str_replace() {
    char *result;
    
    // Test normal case
    result = str_replace("hello world", "world", "there");
    ASSERT_NOT_NULL(result);
    ASSERT_STRING_EQUAL("hello there", result);
    free(result);
    
    // Test multiple replacements
    result = str_replace("hello hello hello", "hello", "hi");
    ASSERT_NOT_NULL(result);
    ASSERT_STRING_EQUAL("hi hi hi", result);
    free(result);
    
    // Test substring not found
    result = str_replace("hello world", "foo", "bar");
    ASSERT_NOT_NULL(result);
    ASSERT_STRING_EQUAL("hello world", result);
    free(result);
    
    // Test empty strings
    result = str_replace("hello world", "", "foo");
    ASSERT_NOT_NULL(result);
    ASSERT_STRING_EQUAL("hello world", result);
    free(result);
    
    result = str_replace("hello world", "world", "");
    ASSERT_NOT_NULL(result);
    ASSERT_STRING_EQUAL("hello ", result);
    free(result);
    
    return TEST_PASSED;
}

/**
 * @brief Test path_join function
 */
TestResult test_path_join() {
    char *result;
    
    // Test normal case
    result = path_join("/usr", "bin");
    ASSERT_NOT_NULL(result);
    ASSERT_STRING_EQUAL("/usr/bin", result);
    free(result);
    
    // Test with trailing slash
    result = path_join("/usr/", "bin");
    ASSERT_NOT_NULL(result);
    ASSERT_STRING_EQUAL("/usr/bin", result);
    free(result);
    
    // Test with leading slash
    result = path_join("/usr", "/bin");
    ASSERT_NOT_NULL(result);
    ASSERT_STRING_EQUAL("/usr/bin", result);
    free(result);
    
    // Test with empty strings
    result = path_join("", "bin");
    ASSERT_NOT_NULL(result);
    ASSERT_STRING_EQUAL("bin", result);
    free(result);
    
    result = path_join("/usr", "");
    ASSERT_NOT_NULL(result);
    ASSERT_STRING_EQUAL("/usr", result);
    free(result);
    
    return TEST_PASSED;
}

/**
 * @brief Test path_dirname function
 */
TestResult test_path_dirname() {
    char *result;
    
    // Test normal case
    result = path_dirname("/usr/bin/gcc");
    ASSERT_NOT_NULL(result);
    ASSERT_STRING_EQUAL("/usr/bin", result);
    free(result);
    
    // Test root directory
    result = path_dirname("/usr");
    ASSERT_NOT_NULL(result);
    ASSERT_STRING_EQUAL("/", result);
    free(result);
    
    // Test current directory
    result = path_dirname("gcc");
    ASSERT_NOT_NULL(result);
    ASSERT_STRING_EQUAL(".", result);
    free(result);
    
    return TEST_PASSED;
}

/**
 * @brief Test path_basename function
 */
TestResult test_path_basename() {
    char *result;
    
    // Test normal case
    result = path_basename("/usr/bin/gcc");
    ASSERT_NOT_NULL(result);
    ASSERT_STRING_EQUAL("gcc", result);
    free(result);
    
    // Test root directory
    result = path_basename("/");
    ASSERT_NOT_NULL(result);
    ASSERT_STRING_EQUAL("/", result);
    free(result);
    
    // Test current directory
    result = path_basename("gcc");
    ASSERT_NOT_NULL(result);
    ASSERT_STRING_EQUAL("gcc", result);
    free(result);
    
    return TEST_PASSED;
}

/**
 * @brief Test memory allocation functions
 */
TestResult test_memory_allocation() {
    // Test safe_malloc
    void *ptr = safe_malloc(100);
    ASSERT_NOT_NULL(ptr);
    free(ptr);
    
    // Test safe_calloc
    ptr = safe_calloc(10, 10);
    ASSERT_NOT_NULL(ptr);
    
    // Test that memory is zeroed
    char *cptr = (char *)ptr;
    for (int i = 0; i < 100; i++) {
        ASSERT_EQUAL(0, cptr[i]);
    }
    
    // Test safe_realloc
    ptr = safe_realloc(ptr, 200);
    ASSERT_NOT_NULL(ptr);
    free(ptr);
    
    // Test str_alloc
    char *str = str_alloc(10);
    ASSERT_NOT_NULL(str);
    free(str);
    
    return TEST_PASSED;
}

/**
 * @brief Test expand_tilde function
 */
TestResult test_expand_tilde() {
    char *result;
    
    // Test without tilde
    result = expand_tilde("/usr/bin");
    ASSERT_NOT_NULL(result);
    ASSERT_STRING_EQUAL("/usr/bin", result);
    free(result);
    
    // Test with tilde
    char *home = getenv("HOME");
    if (home) {
        result = expand_tilde("~");
        ASSERT_NOT_NULL(result);
        ASSERT_STRING_EQUAL(home, result);
        free(result);
        
        char expected[1024];
        snprintf(expected, sizeof(expected), "%s/bin", home);
        
        result = expand_tilde("~/bin");
        ASSERT_NOT_NULL(result);
        ASSERT_STRING_EQUAL(expected, result);
        free(result);
    }
    
    return TEST_PASSED;
}

/**
 * @brief Register all tests
 * 
 * @param suite Test suite to register with
 */
void register_tests(TestSuite *suite) {
    test_suite_add(suite, "test_safe_strcpy", test_safe_strcpy);
    test_suite_add(suite, "test_safe_strcat", test_safe_strcat);
    test_suite_add(suite, "test_str_trim", test_str_trim);
    test_suite_add(suite, "test_str_split", test_str_split);
    test_suite_add(suite, "test_str_starts_with", test_str_starts_with);
    test_suite_add(suite, "test_str_ends_with", test_str_ends_with);
    test_suite_add(suite, "test_str_replace", test_str_replace);
    test_suite_add(suite, "test_path_join", test_path_join);
    test_suite_add(suite, "test_path_dirname", test_path_dirname);
    test_suite_add(suite, "test_path_basename", test_path_basename);
    test_suite_add(suite, "test_memory_allocation", test_memory_allocation);
    test_suite_add(suite, "test_expand_tilde", test_expand_tilde);
}

/**
 * @brief Main entry point
 */
int main(int argc, char *argv[]) {
    TestSuite suite;
    test_suite_init(&suite, "Utility Tests");
    
    register_tests(&suite);
    
    return test_suite_run(&suite);
}