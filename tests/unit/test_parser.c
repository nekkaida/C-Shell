#include "test_framework.h"
#include "test_runner.h"
#include "../../include/command/parser.h"
#include <string.h>

/**
 * Unit tests for the command parser
 */

// Test basic command parsing
TestResult test_basic_command() {
    const char *input = "echo hello world";
    Command cmd = {0};
    
    int result = parse_command((char*)input, &cmd);
    
    ASSERT_EQUAL(0, result);
    ASSERT_EQUAL(3, cmd.argc);
    ASSERT_NOT_NULL(cmd.argv);
    ASSERT_NOT_NULL(cmd.argv[0]);
    ASSERT_STRING_EQUAL("echo", cmd.argv[0]);
    ASSERT_STRING_EQUAL("hello", cmd.argv[1]);
    ASSERT_STRING_EQUAL("world", cmd.argv[2]);
    ASSERT_NULL(cmd.argv[3]);
    
    free_command(&cmd);
    return TEST_PASSED;
}

// Test command parsing with quotes
TestResult test_quoted_command() {
    const char *input = "echo \"hello world\" 'test string'";
    Command cmd = {0};
    
    int result = parse_command((char*)input, &cmd);
    
    ASSERT_EQUAL(0, result);
    ASSERT_EQUAL(3, cmd.argc);
    ASSERT_NOT_NULL(cmd.argv);
    ASSERT_STRING_EQUAL("echo", cmd.argv[0]);
    ASSERT_STRING_EQUAL("hello world", cmd.argv[1]);
    ASSERT_STRING_EQUAL("test string", cmd.argv[2]);
    
    free_command(&cmd);
    return TEST_PASSED;
}

// Test command parsing with escape sequences
TestResult test_escaped_command() {
    const char *input = "echo hello\\ world \\\"quoted\\\"";
    Command cmd = {0};
    
    int result = parse_command((char*)input, &cmd);
    
    ASSERT_EQUAL(0, result);
    ASSERT_EQUAL(3, cmd.argc);
    ASSERT_NOT_NULL(cmd.argv);
    ASSERT_STRING_EQUAL("echo", cmd.argv[0]);
    ASSERT_STRING_EQUAL("hello world", cmd.argv[1]);
    ASSERT_STRING_EQUAL("\"quoted\"", cmd.argv[2]);
    
    free_command(&cmd);
    return TEST_PASSED;
}

// Test empty command
TestResult test_empty_command() {
    const char *input = "";
    Command cmd = {0};
    
    int result = parse_command((char*)input, &cmd);
    
    ASSERT_EQUAL(0, result);
    ASSERT_EQUAL(0, cmd.argc);
    
    free_command(&cmd);
    return TEST_PASSED;
}

// Test NULL input
TestResult test_null_command() {
    Command cmd = {0};
    
    int result = parse_command(NULL, &cmd);
    
    ASSERT_EQUAL(-1, result);
    
    return TEST_PASSED;
}

// Register all tests
DEFINE_TEST_REGISTER(
    REGISTER_TEST(suite, test_basic_command);
    REGISTER_TEST(suite, test_quoted_command);
    REGISTER_TEST(suite, test_escaped_command);
    REGISTER_TEST(suite, test_empty_command);
    REGISTER_TEST(suite, test_null_command);
)

// Test entry point
TEST_MAIN()