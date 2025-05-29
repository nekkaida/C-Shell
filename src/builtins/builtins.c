/**
 * @file builtins.c
 * @brief Built-in commands implementation
 *
 * Implementation of built-in shell commands.
 */

#include "builtins/builtins.h"
#include "command/command.h"
#include "utils/error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Table of built-in commands
static const BuiltinCommand builtin_commands[] = {
    { "cd",     builtin_cd,     "Change the current directory" },
    { "echo",   builtin_echo,   "Display a line of text" },
    { "exit",   builtin_exit,   "Exit the shell" },
    { "pwd",    builtin_pwd,    "Print the current working directory" },
    { "type",   builtin_type,   "Display information about command type" },
    { "help",   builtin_help,   "Display help for built-in commands" },
    { NULL,     NULL,           NULL }  // End of array marker
};

/**
 * @brief Get the table of built-in commands
 * 
 * @return const BuiltinCommand* Array of built-in commands (NULL-terminated)
 */
const BuiltinCommand *get_builtin_commands(void) {
    return builtin_commands;
}

/**
 * @brief Find a built-in command by name
 * 
 * @param name Command name to search for
 * @return BuiltinFunc Function pointer for the command or NULL if not found
 */
BuiltinFunc find_builtin(const char *name) {
    if (!name) {
        return NULL;
    }
    
    for (int i = 0; builtin_commands[i].name != NULL; i++) {
        if (strcmp(name, builtin_commands[i].name) == 0) {
            return builtin_commands[i].func;
        }
    }
    
    return NULL;
}

/**
 * @brief Check if a command is a built-in
 * 
 * @param name Command name to check
 * @return int 1 if it's a built-in, 0 otherwise
 */
int is_builtin(const char *name) {
    return find_builtin(name) != NULL;
}

/**
 * @brief Execute a built-in command
 * 
 * @param cmd Command structure
 * @return int Exit status from the command
 */
int builtin_dispatch(Command *cmd) {
    if (!cmd || cmd->argc == 0) {
        return -1;
    }
    
    BuiltinFunc func = find_builtin(cmd->argv[0]);
    if (func) {
        return func(cmd);
    }
    
    return -1;
}

/**
 * @brief cd (change directory) built-in
 * 
 * @param cmd Command structure
 * @return int Exit status
 */
int builtin_cd(Command *cmd) {
    if (!cmd) {
        return -1;
    }
    
    char *path = NULL;
    
    // If no arguments, change to home directory
    if (cmd->argc == 1) {
        path = getenv("HOME");
        if (!path) {
            fprintf(stderr, "cd: HOME not set\n");
            return 1;
        }
    } else {
        path = cmd->argv[1];
    }
    
    // Handle ~ at beginning of path
    if (path[0] == '~') {
        char *home = getenv("HOME");
        if (!home) {
            fprintf(stderr, "cd: HOME not set\n");
            return 1;
        }
        
        // If just "~", use home directory
        if (path[1] == '\0') {
            path = home;
        } else if (path[1] == '/') {
            // ~/ - combine home with rest of path
            char *new_path = malloc(strlen(home) + strlen(path) - 1 + 1);
            if (!new_path) {
                ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
                return 1;
            }
            
            strcpy(new_path, home);
            strcat(new_path, path + 1);  // Skip the ~
            
            if (chdir(new_path) != 0) {
                fprintf(stderr, "cd: %s: No such file or directory\n", new_path);
                free(new_path);
                return 1;
            }
            
            free(new_path);
            return 0;
        }
    }
    
    // Change directory
    if (chdir(path) != 0) {
        fprintf(stderr, "cd: %s: No such file or directory\n", path);
        return 1;
    }
    
    return 0;
}

/**
 * @brief echo built-in
 * 
 * @param cmd Command structure
 * @return int Exit status
 */
int builtin_echo(Command *cmd) {
    if (!cmd) {
        return -1;
    }
    
    // Print each argument with a space between
    for (int i = 1; i < cmd->argc; i++) {
        if (i > 1) {
            printf(" ");
        }
        printf("%s", cmd->argv[i]);
    }
    
    // Print a newline
    printf("\n");
    
    return 0;
}

/**
 * @brief exit built-in
 * 
 * @param cmd Command structure
 * @return int Exit status
 */
int builtin_exit(Command *cmd) {
    if (!cmd) {
        return -1;
    }
    
    int status = 0;
    
    // If argument provided, use it as exit code
    if (cmd->argc > 1) {
        char *endptr;
        status = strtol(cmd->argv[1], &endptr, 10);
        
        // Check for invalid argument
        if (*endptr != '\0') {
            fprintf(stderr, "exit: %s: numeric argument required\n", cmd->argv[1]);
            status = 2;  // Standard shell exit code for misuse
        }
    }
    
    // Exit with the given status
    exit(status);
    
    // Never reached
    return status;
}

/**
 * @brief pwd (print working directory) built-in
 * 
 * @param cmd Command structure
 * @return int Exit status
 */
int builtin_pwd(Command *cmd) {
    if (!cmd) {
        return -1;
    }
    
    // Get current working directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        fprintf(stderr, "pwd: unable to get current directory\n");
        return 1;
    }
    
    // Print current directory
    printf("%s\n", cwd);
    
    return 0;
}

/**
 * @brief type (show command type) built-in
 * 
 * @param cmd Command structure
 * @return int Exit status
 */
int builtin_type(Command *cmd) {
    if (!cmd) {
        return -1;
    }
    
    // Need at least one argument
    if (cmd->argc < 2) {
        fprintf(stderr, "type: missing command name\n");
        return 1;
    }
    
    int ret = 0;
    
    // Check each argument
    for (int i = 1; i < cmd->argc; i++) {
        const char *arg = cmd->argv[i];
        
        // Check if it's a built-in command
        if (is_builtin(arg)) {
            printf("%s is a shell builtin\n", arg);
            continue;
        }
        
        // Check if it's an external command
        char *path = getenv("PATH");
        if (!path) {
            fprintf(stderr, "type: PATH not set\n");
            ret = 1;
            continue;
        }
        
        char *path_copy = strdup(path);
        if (!path_copy) {
            ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
            ret = 1;
            continue;
        }
        
        char *dir = strtok(path_copy, ":");
        int found = 0;
        
        while (dir != NULL) {
            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s/%s", dir, arg);
            
            if (access(full_path, X_OK) == 0) {
                printf("%s is %s\n", arg, full_path);
                found = 1;
                break;
            }
            
            dir = strtok(NULL, ":");
        }
        
        free(path_copy);
        
        if (!found) {
            printf("%s not found\n", arg);
            ret = 1;
        }
    }
    
    return ret;
}

/**
 * @brief help built-in
 * 
 * @param cmd Command structure
 * @return int Exit status
 */
int builtin_help(Command *cmd) {
    if (!cmd) {
        return -1;
    }
    
    // If no arguments, show help for all built-ins
    if (cmd->argc == 1) {
        printf("Shell built-in commands:\n");
        
        for (int i = 0; builtin_commands[i].name != NULL; i++) {
            printf("  %-10s %s\n", builtin_commands[i].name, builtin_commands[i].help);
        }
        
        printf("\nType 'help name' to find out more about the function 'name'.\n");
        return 0;
    }
    
    // Show help for specific command
    for (int i = 1; i < cmd->argc; i++) {
        const char *name = cmd->argv[i];
        int found = 0;
        
        for (int j = 0; builtin_commands[j].name != NULL; j++) {
            if (strcmp(name, builtin_commands[j].name) == 0) {
                printf("%s: %s\n", builtin_commands[j].name, builtin_commands[j].help);
                found = 1;
                break;
            }
        }
        
        if (!found) {
            fprintf(stderr, "help: no help topics match '%s'\n", name);
            return 1;
        }
    }
    
    return 0;
}