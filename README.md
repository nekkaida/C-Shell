# C Shell Implementation

A robust and feature-rich shell implementation in C with advanced features like command completion, I/O redirection, and built-in commands.

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

## Building

### Prerequisites
- GCC or compatible C compiler
- POSIX-compliant operating system
- Make build system

### Compilation
```bash
# Clone the repository
git clone [repository-url]
cd c-shell

# Build the shell
make

# For debug build
make debug

# Clean build files
make clean
```

## Usage

```bash
# Start the shell
./shell

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

## Implementation Details

### Code Structure
- `main.c`: Core shell implementation
- Command parsing and execution
- Terminal handling and raw mode
- Tab completion system
- I/O redirection handling

### Key Components
- Command structure for parsing and execution
- Redirection info structure for I/O management
- Path resolution system
- Tab completion with prefix matching
- Raw mode terminal handling

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Inspired by UNIX shell implementations
- Built with POSIX standards in mind
- Terminal handling based on standard TTY interfaces