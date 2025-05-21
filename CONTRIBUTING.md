# Contributing to C Shell

Thank you for your interest in contributing to the C Shell project! This document provides guidelines and instructions for contributing to make the process smooth and effective for everyone involved.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [Coding Standards](#coding-standards)
- [Commit Guidelines](#commit-guidelines)
- [Pull Request Process](#pull-request-process)
- [Testing](#testing)
- [Documentation](#documentation)
- [Issue Reporting](#issue-reporting)
- [Feature Requests](#feature-requests)

## Code of Conduct

This project adheres to a Code of Conduct that establishes a harassment-free experience for everyone. By participating, you are expected to uphold this code.

- Be respectful and inclusive
- Exercise empathy and kindness
- Provide and accept constructive feedback
- Focus on what is best for the community

## Getting Started

### Prerequisites

- GCC or compatible C compiler
- CMake (version 3.10 or higher)
- POSIX-compliant operating system
- Git version control

### Setting Up Development Environment

1. Fork the repository on GitHub
2. Clone your fork locally:
   ```bash
   git clone https://github.com/YOUR-USERNAME/C-Shell.git
   cd C-Shell
   ```
3. Add the original repository as upstream:
   ```bash
   git remote add upstream https://github.com/username/C-Shell.git
   ```
4. Create a new branch for your work:
   ```bash
   git checkout -b feature/your-feature-name
   ```
5. Build the project:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

## Development Workflow

1. Keep your branch updated with the main repository:
   ```bash
   git fetch upstream
   git rebase upstream/main
   ```
2. Make your changes following the [Coding Standards](#coding-standards)
3. Build and test your changes:
   ```bash
   cmake --build build
   cd build && ctest
   ```
4. Commit your changes following the [Commit Guidelines](#commit-guidelines)
5. Push your branch to your fork:
   ```bash
   git push origin feature/your-feature-name
   ```
6. Submit a Pull Request following the [Pull Request Process](#pull-request-process)

## Coding Standards

### C Code Style

- Use 4 spaces for indentation (no tabs)
- Maximum line length of 80 characters
- Function names in snake_case
- Variable names in snake_case
- Constants and macros in UPPER_CASE
- Struct names with CamelCase
- Always include braces for control structures, even for single-line statements
- Comments should be in C style (/* */) for multi-line and C++ style (//) for single-line

### Memory Management

- Always check the return value of memory allocation functions
- Free all allocated memory before function return
- Use clear ownership rules for dynamically allocated memory
- Document who is responsible for freeing memory in function comments

### Error Handling

- Always check return values of system calls
- Provide meaningful error messages using perror() or strerror()
- Implement proper resource cleanup in error paths
- Use a consistent error reporting strategy

### Documentation

- Document all functions with a brief description, parameters, return values, and error conditions
- Add comments for complex logic or non-obvious behavior
- Keep comments up-to-date with code changes
- Use Doxygen-style comments for API documentation

## Commit Guidelines

- Use the present tense ("Add feature" not "Added feature")
- Use the imperative mood ("Move cursor to..." not "Moves cursor to...")
- Limit the first line to 72 characters or less
- Reference issues and pull requests liberally after the first line
- Use a consistent format:
  ```
  Category: Brief description (72 chars or less)

  More detailed explanatory text, if necessary. Wrap it to about 72
  characters. The blank line separating the summary from the body is
  critical.

  Issue: #123
  ```

Categories include:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation only changes
- `style`: Changes that do not affect the meaning of the code
- `refactor`: Code change that neither fixes a bug nor adds a feature
- `perf`: Code change that improves performance
- `test`: Adding missing tests or correcting existing tests
- `build`: Changes to the build system or external dependencies
- `ci`: Changes to CI configuration files and scripts

## Pull Request Process

1. Update the README.md and other documentation with details of changes if appropriate
2. Update the CHANGELOG.md with details of changes
3. Include relevant tests for your changes
4. Ensure all tests pass and your code follows the project's coding standards
5. Title your PR clearly with a summary of changes
6. Provide a detailed description of the changes and the motivation for them
7. Reference any relevant issues

Pull requests require review from at least one maintainer before being merged.

## Testing

- Write unit tests for new functionality using the project's testing framework
- Run existing tests to ensure your changes don't break existing functionality
- Test on multiple platforms if possible
- Include both positive and negative test cases
- Document test approaches for complex features

## Documentation

- Update the README.md for user-facing changes
- Update the DESIGN.md for architectural changes
- Document new features including:
  - Purpose and use cases
  - Technical implementation details
  - Examples of usage
- Keep API documentation in sync with code

## Issue Reporting

When reporting issues, please include:

1. A clear and descriptive title
2. Detailed steps to reproduce the issue
3. Expected behavior
4. Actual behavior
5. System information (OS, compiler version, etc.)
6. Any relevant logs or output
7. Potential solutions or workarounds if you have them

## Feature Requests

Feature requests are welcome. Please provide:

1. A clear and detailed description of the feature
2. The motivation for the feature
3. Potential implementation approaches
4. How the feature aligns with the project's goals

## Project Structure

The C Shell project is organized as follows:

- Core Components:
  - Command Processor (parsing, tokenization)
  - Execution Engine (process creation, command execution)
  - Terminal Handler (raw mode, input processing)

- Data Structures:
  - Command structure
  - RedirectionInfo structure

- Key Features:
  - Command parsing with quote and escape handling
  - Path resolution
  - Tab completion
  - I/O redirection
  - Built-in commands

When contributing, please ensure your changes align with this structure and extend it in a consistent manner.

## Future Roadmap

The project roadmap includes the following planned features:

1. Job Control
   - Background processes
   - Process groups
   - Job status tracking

2. Command History
   - History storage
   - Search functionality
   - Persistent history

3. Advanced Features
   - Command aliases
   - Environment variable expansion
   - Wildcard expansion

Contributions aligned with these goals are particularly welcome.

---

Thank you for contributing to C Shell! Your efforts help make this project better for everyone.