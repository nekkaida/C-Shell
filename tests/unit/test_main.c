/**
 * @file test_main.c
 * @brief Main test runner for all tests
 */

#include "framework/test_framework.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

// Function declarations for test registration from each module
extern void register_utils_tests(TestSuite *suite);
extern void register_parser_tests(TestSuite *suite);
extern void register_executor_tests(TestSuite *suite);
extern void register_redirection_tests(TestSuite *suite);
extern void register_builtins_tests(TestSuite *suite);
extern void register_completion_tests(TestSuite *suite);
extern void register_terminal_tests(TestSuite *suite);

// Define structure for test runners
typedef struct {
    const char *name;
    const char *path;
} TestRunner;

// List of test runners (each test file is run separately)
static const TestRunner test_runners[] = {
    { "Utils Tests", "bin/test_utils" },
    { "Parser Tests", "bin/test_parser" },
    { "Executor Tests", "bin/test_executor" },
    { "Redirection Tests", "bin/test_redirection" },
    { "Builtins Tests", "bin/test_builtins" },
    { "Completion Tests", "bin/test_completion" },
    { "Terminal Tests", "bin/test_terminal" },
    { NULL, NULL }  // End of list
};

/**
 * @brief Run a single test
 * 
 * @param runner Test runner to run
 * @return int Exit status
 */
int run_test(const TestRunner *runner) {
    printf("\n=== Running %s ===\n", runner->name);
    
    pid_t pid = fork();
    
    if (pid < 0) {
        // Fork failed
        printf("Error: Failed to fork process for %s\n", runner->name);
        return -1;
    } else if (pid == 0) {
        // Child process - run the test
        execl(runner->path, runner->path, NULL);
        
        // If execl returns, there was an error
        printf("Error: Failed to execute %s\n", runner->path);
        exit(1);
    } else {
        // Parent process - wait for child to complete
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else {
            printf("Error: Test %s did not exit normally\n", runner->name);
            return -1;
        }
    }
}

/**
 * @brief Main entry point
 */
int main(int argc, char *argv[]) {
    printf("=== C Shell Test Suite ===\n");
    
    int total_tests = 0;
    int passed_tests = 0;
    int failed_tests = 0;
    
    // Run each test
    for (int i = 0; test_runners[i].name != NULL; i++) {
        total_tests++;
        
        int status = run_test(&test_runners[i]);
        
        if (status == 0) {
            passed_tests++;
        } else {
            failed_tests++;
            printf("Test %s failed with status %d\n", test_runners[i].name, status);
        }
    }
    
    // Print summary
    printf("\n=== Test Summary ===\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed: %d\n", passed_tests);
    printf("Failed: %d\n", failed_tests);
    
    return (failed_tests > 0) ? 1 : 0;
}