/**
 * @file executor.c
 * @brief Command execution implementation
 *
 * Implementation of command execution functions.
 */

#include "command/executor.h"
#include "command/command.h"
#include "builtins/builtins.h"
#include "utils/error.h"
#include "utils/path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

/**
 * @brief Execute a built-in command
 * 
 * Attempts to execute the command as a built-in shell command.
 * 
 * @param cmd Pointer to Command structure
 * @return int 1 if command was executed as a built-in, 0 otherwise
 */
int execute_builtin_command(Command *cmd) {
    if (!cmd || cmd->argc == 0) return 0;
    
    // Check if command is a built-in
    BuiltinFunc builtin = find_builtin(cmd->argv[0]);
    if (builtin) {
        // Execute the built-in command
        builtin(cmd);
        return 1;
    }
    
    // Not a built-in command
    return 0;
}

/**
 * @brief Execute an external command
 * 
 * Executes the command as an external program.
 * 
 * @param cmd Pointer to Command structure
 * @return int Exit status of the command, -1 on error
 */
int execute_external_command(Command *cmd) {
    if (!cmd || cmd->argc == 0) return -1;
    
    // Find the executable
    char *executable_path = find_executable(cmd->argv[0]);
    if (!executable_path) {
        fprintf(stderr, "%s: command not found\n", cmd->argv[0]);
        return -1;
    }
    
    // Create a child process
    pid_t pid = fork();
    
    if (pid < 0) {
        // Fork failed
        ERROR_SYSTEM(ERR_SYSTEM, "Failed to fork process");
        free(executable_path);
        return -1;
    } else if (pid == 0) {
        // Child process - execute the command
        execv(executable_path, cmd->argv);
        
        // If execv returns, it means there was an error
        ERROR_SYSTEM(ERR_SYSTEM, "Failed to execute %s", cmd->argv[0]);
        free(executable_path);
        exit(1);
    } else {
        // Parent process - wait for child to complete
        int status;
        waitpid(pid, &status, 0);
        free(executable_path);
        return WEXITSTATUS(status);
    }
    
    return 0;
}

/**
 * @brief Find an executable in PATH
 * 
 * Searches for an executable in the directories specified by PATH.
 * 
 * @param command Command name to search for
 * @return char* Path to executable (dynamically allocated, caller must free) or NULL if not found
 */
char *find_executable(const char *command) {
    if (!command) return NULL;
    
    // If command contains a slash, it's a path - just check if it's executable
    if (strchr(command, '/') != NULL) {
        if (access(command, F_OK | X_OK) == 0) {
            return strdup(command);
        }
        return NULL;
    }
    
    // Otherwise, search in PATH
    char *path_env = getenv("PATH");
    if (!path_env) return NULL;
    
    char *path_copy = strdup(path_env);
    if (!path_copy) {
        ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
        return NULL;
    }
    
    char *path = strtok(path_copy, ":");
    char *result = NULL;
    
    while (path != NULL) {
        // Build the full path
        size_t path_len = strlen(path);
        size_t cmd_len = strlen(command);
        size_t full_len = path_len + cmd_len + 2; // +2 for '/' and null terminator
        
        char *full_path = malloc(full_len);
        if (!full_path) {
            ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
            continue;
        }
        
        snprintf(full_path, full_len, "%s/%s", path, command);
        
        // Check if file exists and is executable
        if (access(full_path, F_OK | X_OK) == 0) {
            result = full_path;
            break;
        }
        
        free(full_path);
        path = strtok(NULL, ":");
    }
    
    free(path_copy);
    return result;
}

/**
 * @brief Execute a command
 * 
 * Execute a command, either built-in or external.
 * 
 * @param cmd Pointer to Command structure
 * @return int Exit status of the command
 */
int execute_command(Command *cmd) {
    if (!cmd || cmd->argc == 0) return -1;
    
    // First try to execute as a built-in command
    if (execute_builtin_command(cmd)) {
        return 0; // Built-in commands always return 0 for now
    }
    
    // Otherwise, execute as an external command
    return execute_external_command(cmd);
}

/**
 * @brief Create a child process for command execution
 * 
 * Forks a child process for executing external commands.
 * 
 * @param cmd Pointer to Command structure
 * @return int Process ID of the child, -1 on error
 */
pid_t create_process(Command *cmd) {
    if (!cmd || cmd->argc == 0) return -1;
    
    // Find the executable
    char *executable_path = find_executable(cmd->argv[0]);
    if (!executable_path) {
        fprintf(stderr, "%s: command not found\n", cmd->argv[0]);
        return -1;
    }
    
    // Create a child process
    pid_t pid = fork();
    
    if (pid < 0) {
        // Fork failed
        ERROR_SYSTEM(ERR_SYSTEM, "Failed to fork process");
        free(executable_path);
        return -1;
    } else if (pid == 0) {
        // Child process - execute the command
        execv(executable_path, cmd->argv);
        
        // If execv returns, it means there was an error
        ERROR_SYSTEM(ERR_SYSTEM, "Failed to execute %s", cmd->argv[0]);
        free(executable_path);
        exit(1);
    }
    
    // Parent process
    free(executable_path);
    return pid;
}

/**
 * @brief Wait for a child process to complete
 * 
 * Waits for a child process to exit and returns its status.
 * 
 * @param pid Process ID to wait for
 * @return int Exit status of the process
 */
int wait_for_process(pid_t pid) {
    if (pid <= 0) return -1;
    
    int status;
    if (waitpid(pid, &status, 0) == -1) {
        ERROR_SYSTEM(ERR_SYSTEM, "Failed to wait for process");
        return -1;
    }
    
    return WEXITSTATUS(status);
}

/**
 * @brief Execute command in child process
 * 
 * Executes a command in a child process using execv.
 * This function does not return if successful.
 * 
 * @param cmd Pointer to Command structure
 * @param path Path to executable
 * @return int -1 on error (only returns on error)
 */
int exec_command(Command *cmd, const char *path) {
    if (!cmd || !path) return -1;
    
    // Execute the command
    execv(path, cmd->argv);
    
    // If execv returns, there was an error
    ERROR_SYSTEM(ERR_SYSTEM, "Failed to execute %s", cmd->argv[0]);
    return -1;
}

/**
 * @brief Check if a path is executable
 * 
 * Checks if a file exists and is executable.
 * 
 * @param path Path to check
 * @return int 1 if executable, 0 otherwise
 */
int is_executable(const char *path) {
    if (!path) return 0;
    
    return access(path, F_OK | X_OK) == 0;
}