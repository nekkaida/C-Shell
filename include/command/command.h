/**
 * @file command.h
 * @brief Command processing component
 *
 * Defines structures and functions for command parsing and execution.
 */

#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Redirection information structure
 * 
 * Holds information about I/O redirection for a command.
 */
typedef struct {
    char *stdout_file;    /**< Filename for stdout redirection */
    char *stderr_file;    /**< Filename for stderr redirection */
    int append_stdout;    /**< Flag for stdout append mode */
    int append_stderr;    /**< Flag for stderr append mode */
} RedirectionInfo;

/**
 * @brief Command structure
 * 
 * Holds parsed command data including arguments and redirection info.
 */
typedef struct {
    char **argv;          /**< Command arguments (NULL terminated) */
    int argc;             /**< Argument count */
    RedirectionInfo redir; /**< Redirection information */
} Command;

/**
 * @brief Initialize redirection info
 * 
 * Sets initial values for a RedirectionInfo structure.
 * 
 * @param redir Pointer to RedirectionInfo structure
 */
void initialize_redirection_info(RedirectionInfo *redir);

/**
 * @brief Free redirection info resources
 * 
 * Frees memory allocated for a RedirectionInfo structure.
 * 
 * @param redir Pointer to RedirectionInfo structure
 */
void free_redirection_info(RedirectionInfo *redir);

/**
 * @brief Free command resources
 * 
 * Frees memory allocated for a Command structure.
 * 
 * @param cmd Pointer to Command structure
 */
void free_command(Command *cmd);

/**
 * @brief Parse command into argv array
 * 
 * Parses a command string into a Command structure with argc/argv.
 * Handles quotes and escape characters.
 * 
 * @param input Input string to parse
 * @param cmd Pointer to Command structure to fill
 * @return int 0 on success, non-zero on error
 */
int parse_command(char *input, Command *cmd);

/**
 * @brief Parse redirections
 * 
 * Extracts redirection operators from input and sets up RedirectionInfo.
 * Modifies input string to remove redirection operators.
 * 
 * @param input Input string to parse (will be modified)
 * @param redir Pointer to RedirectionInfo structure to fill
 * @return int 0 on success, non-zero on error
 */
int parse_redirections(char *input, RedirectionInfo *redir);

/**
 * @brief Set up redirections
 * 
 * Sets up file descriptors for redirection before command execution.
 * 
 * @param redir Pointer to RedirectionInfo structure
 * @param backup_fds Array to store backup file descriptors (must be size 2)
 * @param new_fds Array to store new file descriptors (must be size 2)
 * @return int 0 on success, non-zero on error
 */
int setup_redirections(RedirectionInfo *redir, int *backup_fds, int *new_fds);

/**
 * @brief Restore redirections
 * 
 * Restores original file descriptors after command execution.
 * 
 * @param backup_fds Array of backup file descriptors (size 2)
 * @param new_fds Array of new file descriptors (size 2)
 */
void restore_redirections(int *backup_fds, int *new_fds);

/**
 * @brief Execute a built-in command
 * 
 * Attempts to execute the command as a built-in shell command.
 * 
 * @param cmd Pointer to Command structure
 * @return int 1 if command was executed as a built-in, 0 otherwise
 */
int execute_builtin_command(Command *cmd);

/**
 * @brief Execute an external command
 * 
 * Executes the command as an external program.
 * 
 * @param cmd Pointer to Command structure
 * @return int Exit status of the command, -1 on error
 */
int execute_external_command(Command *cmd);

#endif /* COMMAND_H */