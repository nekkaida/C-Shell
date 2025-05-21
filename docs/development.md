# C Shell Development Guide

This document provides guidance for developers who want to contribute to or extend the C Shell project.

## Development Setup

### Prerequisites

- GCC or compatible C compiler
- CMake (version 3.10 or higher)
- POSIX-compliant operating system
- Git version control
- Optional: Valgrind for memory leak detection
- Optional: cppcheck for static analysis

### Getting the Source Code

```bash
# Clone the repository
git clone https://github.com/username/C-Shell.git
cd C-Shell

# Create a branch for your changes
git checkout -b feature/your-feature-name
```

### Building for Development

```bash
# Using CMake (recommended)
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Or using Makefile
make debug
```

### Running the Shell

```bash
# From CMake build directory
./bin/shell

# Or from project root with Makefile
./bin/shell
```

## Project Structure

The project is organized as follows:

```
c-shell/
├── src/          # Source code
│   ├── builtins/ # Built-in command implementations
│   ├── command/  # Command parsing and execution
│   ├── completion/ # Tab completion
│   ├── terminal/ # Terminal handling
│   ├── utils/    # Utility functions
│   └── main.c    # Main entry point
├── include/      # Header files
├── tests/        # Test suite
├── docs/         # Documentation
└── tools/        # Developer tools
```

## Development Workflow

1. Create a feature branch
2. Make your changes
3. Write tests for your changes
4. Ensure all tests pass
5. Update documentation
6. Submit a pull request

### Coding Standards

- Follow the existing code style (4-space indentation, etc.)
- Use meaningful variable and function names
- Add comments for complex logic
- Document all functions with parameters, return values, and behavior
- Check for memory leaks using Valgrind
- Run static analysis with cppcheck

## Testing

### Running Tests

```bash
# Using CMake
cd build
ctest

# Or using Makefile
make test
```

### Adding Tests

1. Create a new test file in `tests/unit/` or `tests/integration/`
2. For unit tests, include the test framework and implement test cases
3. For integration tests, add test cases to the appropriate script
4. Update test runner configuration if necessary

### Test Guidelines

- Test both success and failure cases
- Test edge cases (empty input, very long input, etc.)
- Cover all code paths
- Keep tests independent of each other
- Clean up after tests

## Debugging

### Debug Builds

Debug builds include additional debugging information and disable optimizations:

```bash
# Using CMake
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Or using Makefile
make debug
```

### Using GDB

```bash
# Start the shell under GDB
gdb ./bin/shell

# Set breakpoints
(gdb) break parse_command
(gdb) break execute_external_command

# Run the program
(gdb) run

# Step through code
(gdb) step
(gdb) next
```

### Memory Leak Detection

```bash
valgrind --leak-check=full ./bin/shell
```

## Implementation Guidelines

### Memory Management

- Always check the return value of memory allocation functions
- Free all allocated memory
- Document ownership of allocated memory
- Use defensive programming (check for NULL pointers, etc.)

### Error Handling

- Check return values of system calls
- Use perror() or strerror() for error messages
- Clean up resources in error paths
- Return meaningful error codes

### Security Considerations

- Validate input
- Use safe string functions
- Be careful with user-provided paths
- Check permissions before accessing files

## Adding Features

### Adding a New Built-in Command

1. Create a new file in `src/builtins/` for your command
2. Implement the command function
3. Add the command to the dispatch table in `builtins.c`
4. Update the tab completion to include your command
5. Add tests for your command
6. Update documentation

### Adding a New Shell Feature

1. Design the feature and its API
2. Create new files as needed
3. Implement the feature
4. Integrate the feature with the shell
5. Add tests for the feature
6. Update documentation

## Release Process

1. Update the CHANGELOG.md file
2. Update version numbers
3. Run all tests
4. Create a release branch
5. Tag the release
6. Create release notes

## Getting Help

- Check the existing documentation
- Look at the code for similar functionality
- Ask questions in the issue tracker
- Consult POSIX shell documentation for reference

## Common Tasks

### Fixing a Bug

1. Create an issue describing the bug
2. Create a branch for the fix
3. Write a test that reproduces the bug
4. Fix the bug
5. Ensure the test passes
6. Submit a pull request

### Adding a New Test

1. Create a new test file or add to an existing one
2. Implement the test cases
3. Run the tests to ensure they pass
4. Submit a pull request

### Improving Documentation

1. Identify documentation that needs improvement
2. Make the changes
3. Submit a pull request

## Advanced Topics

### Signal Handling

The shell needs to handle various signals:

- SIGINT (Ctrl+C): Interrupt the current command
- SIGTSTP (Ctrl+Z): Stop the current command
- SIGCHLD: Child process terminated

### Job Control

For implementing job control:

- Process groups
- Foreground/background processes
- Job table
- Job status reporting

### Command History

For implementing command history:

- History storage
- History navigation (up/down arrows)
- History search
- Persistent history file