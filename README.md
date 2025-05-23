# C Shell Implementation

A robust and feature-rich shell implementation in C with advanced features like command completion, I/O redirection, and built-in commands.

[![Build Status](https://github.com/username/c-shell/actions/workflows/build.yml/badge.svg)](https://github.com/username/c-shell/actions/workflows/build.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

## Features

### Command Execution
- Execute external commands with proper PATH resolution
- Built-in commands support (`cd`, `echo`, `pwd`, `type`, `exit`)
- Command path resolution and executable finding
- Process creation and management using `fork()` and `execv()`

### Input/Output Features
- I/O redirection support (`>` and `>>` operators)
- Stderr redirection (`2>` and `2>>`)
- Raw mode terminal handling for better input control
- Tab completion with intelligent suggestions
- Command history navigation (WIP)

### Built-in Commands
- `cd [directory]`: Change current directory
  - Supports `~` for home directory
  - Supports relative and absolute paths
- `echo [text...]`: Print text to stdout
  - Supports escape sequences
  - Handles quoted strings
- `pwd`: Print working directory
- `type [command]`: Display command type
- `exit [code]`: Exit shell with optional status code

### Advanced Features
- Intelligent tab completion
  - Command suggestions from PATH
  - Built-in command completion
  - Double-tab to show all possibilities
- Memory-safe string operations
- Proper error handling and reporting
- Clean terminal handling with raw mode

## Project Structure

```
c-shell/
├── src/          # Source code
├── include/      # Header files
├── tests/        # Test suite
├── docs/         # Documentation
└── tools/        # Developer tools
```

## Getting Started

### Prerequisites
- GCC or compatible C compiler
- CMake (version 3.10 or higher)
- POSIX-compliant operating system

### Building from Source

Using CMake (recommended):
```bash
mkdir build && cd build
cmake ..
make
```

Using Makefile:
```bash
make
```

### Running the Shell

```bash
./bin/shell
```

## Usage Examples

```bash
# Basic command usage
$ ls -l
$ pwd
$ cd ~/Documents
$ echo "Hello World"

# I/O redirection
$ ls > output.txt
$ echo "append" >> output.txt
$ ls -l 2> error.log

# Tab completion
$ ec[TAB]     # Completes to "echo"
$ ls Doc[TAB] # Completes directory names
```

## Development

### Building for Development

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

### Running Tests

```bash
cd build
ctest
```

### Code Style and Linting

```bash
# Check code style
clang-format --dry-run --Werror src/*.c include/*.h

# Run static analysis
cppcheck --enable=warning,performance,portability src/
```

## Contributing

Contributions are welcome! Please check out our [Contributing Guide](CONTRIBUTING.md) for details on:

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## Roadmap

Upcoming features:
- Job control (background processes)
- Command history with search
- Environment variable expansion
- Wildcard support
- Command aliases

See the [DESIGN.md](DESIGN.md) file for more details on the roadmap and architecture.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by UNIX shell implementations
- Built with POSIX standards in mind
- Terminal handling based on standard TTY interfaces