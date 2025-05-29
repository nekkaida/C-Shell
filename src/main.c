/**
 * @file main.c
 * @brief Main entry point for C Shell
 *
 * This file contains the main function and shell initialization.
 */

#include "shell.h"
#include "terminal/terminal.h"
#include "utils/error.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

/** Global verbose flag */
int g_verbose_mode = 0;

/**
 * @brief Signal handler for SIGINT
 * 
 * Handles Ctrl+C (SIGINT) signal.
 * 
 * @param sig Signal number
 */
static void sigint_handler(int sig) {
    // Just ignore the signal and redisplay the prompt
    printf("\n");
    display_prompt();
    fflush(stdout);
}

/**
 * @brief Print usage information
 */
static void print_usage(const char *program_name) {
    printf("Usage: %s [OPTIONS]\n\n", program_name);
    printf("Options:\n");
    printf("  -h, --help     Display this help message\n");
    printf("  -v, --verbose  Enable verbose output\n");
    printf("  -V, --version  Display version information\n");
    printf("\n");
}

/**
 * @brief Print version information
 */
static void print_version(void) {
    printf("%s version %s\n", SHELL_NAME, SHELL_VERSION);
    printf("A robust shell implementation in C\n");
}

/**
 * @brief Parse command line arguments
 * 
 * @param argc Argument count
 * @param argv Argument vector
 * @return int 0 on success, non-zero on error
 */
static int parse_args(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            exit(EXIT_SUCCESS);
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            g_verbose_mode = 1;
            error_set_verbose(1);
        } else if (strcmp(argv[i], "-V") == 0 || strcmp(argv[i], "--version") == 0) {
            print_version();
            exit(EXIT_SUCCESS);
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }
    
    return 0;
}

/**
 * @brief Initialize the shell
 * 
 * @return int 0 on success, non-zero on error
 */
int shell_init(void) {
    // Set up signal handlers
    signal(SIGINT, sigint_handler);
    
    // Initialize terminal
    if (terminal_init() != 0) {
        ERROR_ERROR(ERR_SYSTEM, "Failed to initialize terminal");
        return 1;
    }
    
    ERROR_DEBUG("Shell initialized");
    
    return 0;
}

/**
 * @brief Clean up shell resources
 */
void shell_cleanup(void) {
    // Restore terminal settings
    terminal_cleanup();
    
    ERROR_DEBUG("Shell cleaned up");
}

/**
 * @brief Main shell loop
 * 
 * Handles the main loop of reading input, processing commands,
 * and displaying the prompt.
 * 
 * @return int Exit status
 */
int shell_loop(void) {
    char input[INPUT_BUFFER_SIZE];
    int status = 0;
    
    while (1) {
        // Display prompt
        display_prompt();
        
        // Read input
        if (read_line(input, INPUT_BUFFER_SIZE) < 0) {
            if (feof(stdin)) {
                // End of file (Ctrl+D)
                printf("\n");
                break;
            }
            ERROR_ERROR(ERR_IO, "Failed to read input");
            continue;
        }
        
        // Process command
        status = process_command(input);
        
        // Check for exit
        if (status < 0) {
            break;
        }
    }
    
    return status;
}

/**
 * @brief Main function
 * 
 * @param argc Argument count
 * @param argv Argument vector
 * @return int Exit status
 */
int main(int argc, char *argv[]) {
    int status;
    
    // Parse command line arguments
    if (parse_args(argc, argv) != 0) {
        return EXIT_FAILURE;
    }
    
    // Initialize shell
    if (shell_init() != 0) {
        return EXIT_FAILURE;
    }
    
    // Run shell loop
    status = shell_loop();
    
    // Clean up
    shell_cleanup();
    
    return status;
}