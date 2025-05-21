# C Shell API Documentation

This document provides information about the C Shell's internal API for developers who want to modify or extend the shell.

## Core Data Structures

### Command Structure

The `Command` structure represents a parsed command:

```c
typedef struct {
    char **argv;          // Command arguments (NULL terminated)
    int argc;             // Argument count
    RedirectionInfo redir; // Redirection information
} Command;
```

- `argv`: Array of strings containing the command and its arguments (NULL-terminated)
- `argc`: Number of arguments (including the command)
- `redir`: Redirection information

### RedirectionInfo Structure

The `RedirectionInfo` structure contains information about I/O redirection:

```c
typedef struct {
    char *stdout_file;    // Filename for stdout redirection
    char *stderr_file;    // Filename for stderr redirection
    int append_stdout;    // Flag for stdout append mode
    int append_stderr;    // Flag for stderr append mode
} RedirectionInfo;
```

- `stdout_file`: Filename for standard output redirection (NULL if none)
- `stderr_file`: Filename for standard error redirection (NULL if none)
- `append_stdout`: Flag indicating whether to append to stdout file (1) or overwrite (0)
- `append_stderr`: Flag indicating whether to append to stderr file (1) or overwrite (0)

## Command Processing

### Command Parsing

```c
int parse_command(char *input, Command *cmd);
```

Parses a command string into a `Command` structure.

**Parameters:**
- `input`: Input string to parse
- `cmd`: Pointer to a Command structure to fill

**Returns:**
- 0 on success
- -1 on error (memory allocation failure, NULL input, etc.)

### Redirection Parsing

```c
int parse_redirections(char *input, RedirectionInfo *redir);
```

Parses redirection operators from a command string.

**Parameters:**
- `input`: Input string to parse
- `redir`: Pointer to a RedirectionInfo structure to fill

**Returns:**
- 0 on success
- -1 on error

**Note:** This function modifies the input string by removing redirection operators.

### Command Execution

```c
int execute_builtin_command(Command *cmd);
```

Attempts to execute a built-in command.

**Parameters:**
- `cmd`: Pointer to a Command structure

**Returns:**
- 1 if the command was a built-in and was executed
- 0 if the command was not a built-in

```c
int execute_external_command(Command *cmd);
```

Executes an external command.

**Parameters:**
- `cmd`: Pointer to a Command structure

**Returns:**
- Exit status of the command
- -1 on error

## Memory Management

### Command Initialization

```c
void initialize_redirection_info(RedirectionInfo *redir);
```

Initializes a RedirectionInfo structure.

**Parameters:**
- `redir`: Pointer to a RedirectionInfo structure to initialize

### Command Cleanup

```c
void free_command(Command *cmd);
```

Frees resources used by a Command structure.

**Parameters:**
- `cmd`: Pointer to a Command structure to free

```c
void free_redirection_info(RedirectionInfo *redir);
```

Frees resources used by a RedirectionInfo structure.

**Parameters:**
- `redir`: Pointer to a RedirectionInfo structure to free

## Terminal Handling

### Raw Mode

```c
void enable_raw_mode();
```

Puts the terminal in raw mode (non-canonical, no echo).

```c
void disable_raw_mode();
```

Restores the terminal to its original state.

### Tab Completion

```c
void handle_tab_completion(char *input, int *cursor_pos);
```

Handles tab completion for the current input.

**Parameters:**
- `input`: Current input string
- `cursor_pos`: Pointer to the current cursor position

## Path Handling

```c
char *find_executable(const char *command);
```

Finds the path to an executable.

**Parameters:**
- `command`: Name of the command to find

**Returns:**
- Path to the executable (caller must free)
- NULL if the executable was not found

## String Utilities

```c
char* find_longest_common_prefix(char **strings, int count);
```

Finds the longest common prefix among a set of strings.

**Parameters:**
- `strings`: Array of strings
- `count`: Number of strings in the array

**Returns:**
- The longest common prefix (caller must free)
- Empty string if no common prefix was found

## Error Handling

The shell uses standard error reporting mechanisms:

- Return codes to indicate success or failure
- `perror()` for system call errors
- `fprintf(stderr, ...)` for custom error messages

## Event Loop

The main event loop is in `main()` and handles:

1. Reading input character by character
2. Handling special keys (Enter, Tab, Backspace)
3. Processing commands
4. Maintaining the prompt

## Extending the Shell

### Adding a New Built-in Command

To add a new built-in command:

1. Add a case for your command in `execute_builtin_command()`
2. Implement the command's functionality
3. Add the command to the tab completion list in `get_all_completions()`

### Adding a New Feature

To add a new feature:

1. Update the header files with new function declarations
2. Implement the feature in the appropriate source file
3. Update the main loop if necessary
4. Add tests for the new feature
5. Update documentation