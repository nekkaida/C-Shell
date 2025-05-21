/**
 * @file shell.h
 * @brief Main header file for C Shell
 *
 * This file includes all the necessary components for the shell
 * and defines common structures and macros.
 */

#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <termios.h>
#include <dirent.h>
#include <limits.h>

/* Include all component headers */
#include "command/command.h"
#include "builtins/builtins.h"
#include "terminal/terminal.h"
#include "completion/completion.h"
#include "utils/utils.h"

/**
 * @brief Version information
 */
#define SHELL_VERSION "0.1.0"
#define SHELL_NAME "C-Shell"

/**
 * @brief Error codes
 */
#define SUCCESS 0
#define ERR_GENERAL -1
#define ERR_MEMORY -2
#define ERR_SYNTAX -3
#define ERR_IO -4
#define ERR_COMMAND_NOT_FOUND -5

/**
 * @brief Maximum input buffer size
 */
#define INPUT_BUFFER_SIZE 1024

/**
 * @brief Global verbose flag
 * 
 * Set to 1 to enable verbose output for debugging
 */
extern int g_verbose_mode;

/**
 * @brief Process a command
 * 
 * Main function to process a command: parses the input, sets up redirections,
 * and executes the command.
 * 
 * @param input Command input string
 * @return int Status code from command execution
 */
int process_command(char *input);

/**
 * @brief Main shell loop
 * 
 * Handles the main loop of reading input, processing commands,
 * and displaying the prompt.
 * 
 * @return int Exit status
 */
int shell_loop(void);

/**
 * @brief Initialize the shell
 * 
 * Performs any necessary initialization before starting the shell loop.
 * 
 * @return int 0 on success, non-zero on error
 */
int shell_init(void);

/**
 * @brief Clean up shell resources
 * 
 * Performs any necessary cleanup before exiting the shell.
 */
void shell_cleanup(void);

#endif /* SHELL_H */