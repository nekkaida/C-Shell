# C Shell Usage Guide

This document provides a comprehensive guide to using the C Shell implementation.

## Basic Usage

### Starting the Shell

After building the shell, you can run it using:

```bash
./bin/shell
```

The shell will display a prompt (`$`) where you can enter commands.

### Executing Commands

To run a command, simply type it at the prompt and press Enter:

```bash
$ ls -l
```

The shell will execute the command and display the output.

## Built-in Commands

The C Shell comes with several built-in commands:

### cd - Change Directory

Change the current working directory.

**Usage:**
```bash
$ cd [directory]
```

**Examples:**
```bash
$ cd /usr/local    # Change to absolute path
$ cd ..            # Go up one directory
$ cd ~             # Go to home directory
$ cd               # Same as cd ~
```

### echo - Print Text

Print text to standard output.

**Usage:**
```bash
$ echo [text...]
```

**Examples:**
```bash
$ echo Hello, world!
$ echo "Quoted text"
$ echo 'Single quoted text'
```

### pwd - Print Working Directory

Print the current working directory.

**Usage:**
```bash
$ pwd
```

### type - Display Command Type

Show information about a command.

**Usage:**
```bash
$ type command
```

**Examples:**
```bash
$ type echo     # Shows "echo is a shell builtin"
$ type ls       # Shows "ls is /bin/ls"
```

### exit - Exit the Shell

Exit the shell with an optional status code.

**Usage:**
```bash
$ exit [code]
```

**Examples:**
```bash
$ exit          # Exit with status 0
$ exit 1        # Exit with status 1
```

## I/O Redirection

The shell supports redirecting standard output and standard error to files.

### Output Redirection

Redirect standard output to a file:

```bash
$ command > file       # Overwrite file
$ command >> file      # Append to file
```

### Error Redirection

Redirect standard error to a file:

```bash
$ command 2> file      # Overwrite file
$ command 2>> file     # Append to file
```

### Examples

```bash
$ ls -l > files.txt        # Save directory listing to files.txt
$ echo "Append" >> log.txt  # Append text to log.txt
$ ls nonexistent 2> error.log  # Save error messages to error.log
```

## Tab Completion

The shell provides intelligent tab completion for commands and file paths.

- Press Tab once to complete a command or file if there's a unique match
- Press Tab twice to show all possible completions
- Tab completion works for both built-in commands and executables in PATH

**Examples:**
```bash
$ ec[TAB]               # Completes to "echo "
$ ls /us[TAB]           # Completes to "ls /usr/"
$ ls /usr/[TAB][TAB]    # Shows all entries in /usr/
```

## Tips and Tricks

- Use Up/Down arrow keys to navigate command history (future feature)
- The shell correctly handles quoted strings and escape characters
- You can use `~` as a shorthand for your home directory

## Error Handling

The shell provides informative error messages:

- Command not found errors
- Permission denied errors
- File not found errors

## Advanced Features (Coming Soon)

These features are planned for future releases:

- Command history navigation and search
- Job control (background processes)
- Environment variable expansion
- Wildcard expansion
- Command aliases

## Troubleshooting

Common issues and their solutions:

### Command Not Found

If you receive a "command not found" error, check:
- The command spelling
- That the command exists in your PATH

### Permission Errors

If you get a "permission denied" error:
- Check that you have permission to execute the file
- For scripts, make sure they're executable (`chmod +x script.sh`)

### Other Issues

If you encounter other issues, please report them by opening an issue on the GitHub repository.