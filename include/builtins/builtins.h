/**
 * @file builtins.h
 * @brief Built-in commands
 *
 * Declaration of built-in command functions and dispatch table.
 */

#ifndef BUILTINS_H
#define BUILTINS_H

#include "../command/command.h"

/**
 * @brief Built-in command function signature
 */
typedef int (*BuiltinFunc)(Command *cmd);

/**
 * @brief Built-in command entry structure
 */
typedef struct {
    const char *name;    /**< Command name */
    BuiltinFunc func;    /**< Function pointer */
    const char *help;    /**< Help message */
} BuiltinCommand;

/**
 * @brief Get the table of built-in commands
 * 
 * @return const BuiltinCommand* Array of built-in commands (NULL-terminated)
 */
const BuiltinCommand *get_builtin_commands(void);

/**
 * @brief Find a built-in command by name
 * 
 * @param name Command name to search for
 * @return BuiltinFunc Function pointer for the command or NULL if not found
 */
BuiltinFunc find_builtin(const char *name);

/**
 * @brief Check if a command is a built-in
 * 
 * @param name Command name to check
 * @return int 1 if it's a built-in, 0 otherwise
 */
int is_builtin(const char *name);

/**
 * @brief Execute a built-in command
 * 
 * @param cmd Command structure
 * @return int Exit status from the command
 */
int builtin_dispatch(Command *cmd);

/**
 * @brief cd (change directory) built-in
 * 
 * @param cmd Command structure
 * @return int Exit status
 */
int builtin_cd(Command *cmd);

/**
 * @brief echo built-in
 * 
 * @param cmd Command structure
 * @return int Exit status
 */
int builtin_echo(Command *cmd);

/**
 * @brief exit built-in
 * 
 * @param cmd Command structure
 * @return int Exit status
 */
int builtin_exit(Command *cmd);

/**
 * @brief pwd (print working directory) built-in
 * 
 * @param cmd Command structure
 * @return int Exit status
 */
int builtin_pwd(Command *cmd);

/**
 * @brief type (show command type) built-in
 * 
 * @param cmd Command structure
 * @return int Exit status
 */
int builtin_type(Command *cmd);

/**
 * @brief help built-in
 * 
 * @param cmd Command structure
 * @return int Exit status
 */
int builtin_help(Command *cmd);

#endif /* BUILTINS_H */