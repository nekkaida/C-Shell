/**
 * @file process_command.c
 * @brief Command processing implementation
 *
 * Implementation of the process_command function and related helpers.
 */

#include "shell.h"
#include "command/command.h"
#include "command/parser.h"
#include "command/executor.h"
#include "command/redirection.h"
#include "utils/error.h"
#include "utils/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Process a command
 * 
 * Main function to process a command: parses the input, sets up redirections,
 * and executes the command.
 * 
 * @param input Command input string
 * @return int Status code from command execution
 */
int process_command(char *input) {
    if (input == NULL || *input == '\0') {
        return 0;
    }

    // Create a copy of input that we can modify
    char *input_copy = strdup(input);
    if (input_copy == NULL) {
        ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
        return -1;
    }

    // Initialize command structure
    Command cmd;
    memset(&cmd, 0, sizeof(Command));
    initialize_redirection_info(&cmd.redir);

    // Parse redirections and remove them from the command string
    if (parse_redirections(input_copy, &cmd.redir) != 0) {
        ERROR_ERROR(ERR_PARSE, "Failed to parse redirections");
        free(input_copy);
        free_redirection_info(&cmd.redir);
        return -1;
    }

    // Parse command and arguments
    if (parse_command(input_copy, &cmd) != 0) {
        ERROR_ERROR(ERR_PARSE, "Failed to parse command");
        free(input_copy);
        free_command(&cmd);
        return -1;
    }

    // Free the input copy as it's no longer needed
    free(input_copy);

    // Skip if no command was found
    if (cmd.argc == 0) {
        free_command(&cmd);
        return 0;
    }

    // Setup file descriptor backups and new file descriptors
    int backup_fds[2] = {-1, -1}; // stdout, stderr
    int new_fds[2] = {-1, -1};    // stdout, stderr

    // Set up redirections
    if (setup_redirections(&cmd.redir, backup_fds, new_fds) != 0) {
        ERROR_ERROR(ERR_IO, "Failed to set up redirections");
        free_command(&cmd);
        return -1;
    }

    int status = 0;

    // Try to execute as a built-in command first
    if (!execute_builtin_command(&cmd)) {
        // If not a built-in, try as external command
        status = execute_external_command(&cmd);
        if (status < 0) {
            ERROR_ERROR(ERR_NOT_FOUND, "Command not found: %s", cmd.argv[0]);
        }
    }

    // Restore original stdout/stderr
    restore_redirections(backup_fds, new_fds);

    // Clean up command resources
    free_command(&cmd);

    return status;
}

/**
 * @brief Initialize redirection info structure
 * 
 * Sets initial values for a RedirectionInfo structure.
 * 
 * @param redir Pointer to RedirectionInfo structure
 */
void initialize_redirection_info(RedirectionInfo *redir) {
    if (redir) {
        redir->stdout_file = NULL;
        redir->stderr_file = NULL;
        redir->append_stdout = 0;
        redir->append_stderr = 0;
    }
}

/**
 * @brief Free resources used by redirection info
 * 
 * Frees memory allocated for a RedirectionInfo structure.
 * 
 * @param redir Pointer to RedirectionInfo structure
 */
void free_redirection_info(RedirectionInfo *redir) {
    if (redir) {
        if (redir->stdout_file) free(redir->stdout_file);
        if (redir->stderr_file) free(redir->stderr_file);
        redir->stdout_file = NULL;
        redir->stderr_file = NULL;
    }
}

/**
 * @brief Free resources used by command structure
 * 
 * Frees memory allocated for a Command structure.
 * 
 * @param cmd Pointer to Command structure
 */
void free_command(Command *cmd) {
    if (cmd) {
        if (cmd->argv) {
            for (int i = 0; i < cmd->argc; i++) {
                if (cmd->argv[i]) free(cmd->argv[i]);
            }
            free(cmd->argv);
            cmd->argv = NULL;
        }
        cmd->argc = 0;
        free_redirection_info(&cmd->redir);
    }
}

/**
 * @brief Set up redirections before command execution
 * 
 * Sets up file descriptors for redirection before command execution.
 * 
 * @param redir Pointer to RedirectionInfo structure
 * @param backup_fds Array to store backup file descriptors (must be size 2)
 * @param new_fds Array to store new file descriptors (must be size 2)
 * @return int 0 on success, non-zero on error
 */
int setup_redirections(RedirectionInfo *redir, int *backup_fds, int *new_fds) {
    if (!redir || !backup_fds || !new_fds) return -1;
    
    // Set up redirection for stdout if needed
    if (redir->stdout_file != NULL) {
        // Backup the original stdout
        backup_fds[0] = dup(STDOUT_FILENO);
        if (backup_fds[0] == -1) {
            ERROR_SYSTEM(ERR_IO, "dup failed for stdout");
            return -1;
        }
        
        // Open the output file with appropriate flags based on append_stdout
        int file_flags = O_WRONLY | O_CREAT;
        if (redir->append_stdout) {
            file_flags |= O_APPEND;  // Append to file
        } else {
            file_flags |= O_TRUNC;   // Overwrite file
        }
        new_fds[0] = open(redir->stdout_file, file_flags, 0644);
        
        if (new_fds[0] == -1) {
            ERROR_SYSTEM(ERR_IO, "Failed to open stdout file: %s", redir->stdout_file);
            close(backup_fds[0]);
            backup_fds[0] = -1;
            return -1;
        }
        
        // Redirect stdout to the file
        if (dup2(new_fds[0], STDOUT_FILENO) == -1) {
            ERROR_SYSTEM(ERR_IO, "dup2 failed for stdout");
            close(new_fds[0]);
            close(backup_fds[0]);
            new_fds[0] = -1;
            backup_fds[0] = -1;
            return -1;
        }
    }
    
    // Set up redirection for stderr if needed
    if (redir->stderr_file != NULL) {
        // Backup the original stderr
        backup_fds[1] = dup(STDERR_FILENO);
        if (backup_fds[1] == -1) {
            ERROR_SYSTEM(ERR_IO, "dup failed for stderr");
            
            // Clean up stdout redirection if it was set
            if (backup_fds[0] != -1) {
                dup2(backup_fds[0], STDOUT_FILENO);
                close(backup_fds[0]);
                if (new_fds[0] != -1) close(new_fds[0]);
                backup_fds[0] = -1;
                new_fds[0] = -1;
            }
            return -1;
        }
        
        // Open the error file with appropriate flags based on append_stderr
        int file_flags = O_WRONLY | O_CREAT;
        if (redir->append_stderr) {
            file_flags |= O_APPEND;  // Append to file
        } else {
            file_flags |= O_TRUNC;   // Overwrite file
        }
        new_fds[1] = open(redir->stderr_file, file_flags, 0644);
        
        if (new_fds[1] == -1) {
            ERROR_SYSTEM(ERR_IO, "Failed to open stderr file: %s", redir->stderr_file);
            close(backup_fds[1]);
            backup_fds[1] = -1;
            
            // Clean up stdout redirection if it was set
            if (backup_fds[0] != -1) {
                dup2(backup_fds[0], STDOUT_FILENO);
                close(backup_fds[0]);
                if (new_fds[0] != -1) close(new_fds[0]);
                backup_fds[0] = -1;
                new_fds[0] = -1;
            }
            return -1;
        }
        
        // Redirect stderr to the file
        if (dup2(new_fds[1], STDERR_FILENO) == -1) {
            ERROR_SYSTEM(ERR_IO, "dup2 failed for stderr");
            close(new_fds[1]);
            close(backup_fds[1]);
            new_fds[1] = -1;
            backup_fds[1] = -1;
            
            // Clean up stdout redirection if it was set
            if (backup_fds[0] != -1) {
                dup2(backup_fds[0], STDOUT_FILENO);
                close(backup_fds[0]);
                if (new_fds[0] != -1) close(new_fds[0]);
                backup_fds[0] = -1;
                new_fds[0] = -1;
            }
            return -1;
        }
    }
    
    return 0;
}

/**
 * @brief Restore original stdout/stderr after command execution
 * 
 * Restores file descriptors to their original state after redirection.
 * 
 * @param backup_fds Array of backup file descriptors (size 2)
 * @param new_fds Array of new file descriptors (size 2)
 */
void restore_redirections(int *backup_fds, int *new_fds) {
    // Restore stdout if it was redirected
    if (backup_fds[0] != -1) {
        fflush(stdout);
        dup2(backup_fds[0], STDOUT_FILENO);
        close(backup_fds[0]);
        if (new_fds[0] != -1) close(new_fds[0]);
        backup_fds[0] = -1;
        new_fds[0] = -1;
    }
    
    // Restore stderr if it was redirected
    if (backup_fds[1] != -1) {
        fflush(stderr);
        dup2(backup_fds[1], STDERR_FILENO);
        close(backup_fds[1]);
        if (new_fds[1] != -1) close(new_fds[1]);
        backup_fds[1] = -1;
        new_fds[1] = -1;
    }
}