/**
 * @file terminal.c
 * @brief Terminal handling implementation
 *
 * Implementation of terminal handling functions.
 */

#include "terminal/terminal.h"
#include "terminal/input.h"
#include "utils/error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

/** Original terminal attributes */
static struct termios orig_termios;
static int raw_mode_enabled = 0;

/**
 * @brief Initialize terminal settings
 * 
 * Sets up the terminal for shell operation.
 * 
 * @return int 0 on success, non-zero on error
 */
int terminal_init(void) {
    // Save original terminal attributes
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        if (isatty(STDIN_FILENO)) {
            ERROR_SYSTEM(ERR_SYSTEM, "Failed to get terminal attributes");
            return -1;
        }
        // If not a terminal, just continue
        return 0;
    }
    
    return 0;
}

/**
 * @brief Clean up terminal settings
 * 
 * Restores original terminal settings.
 */
void terminal_cleanup(void) {
    // Restore original terminal attributes if in raw mode
    if (raw_mode_enabled) {
        disable_raw_mode();
    }
}

/**
 * @brief Enable raw mode
 * 
 * Puts the terminal in raw mode for character-by-character input.
 */
void enable_raw_mode(void) {
    if (!isatty(STDIN_FILENO)) {
        return;  // Not a terminal
    }
    
    if (raw_mode_enabled) {
        return;  // Already in raw mode
    }
    
    struct termios raw = orig_termios;
    
    // Input modes: no break, no CR to NL, no parity check, no strip char,
    // no start/stop output control
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    
    // Output modes: disable post processing
    raw.c_oflag &= ~(OPOST);
    
    // Control modes: set 8 bit chars
    raw.c_cflag |= (CS8);
    
    // Local modes: echoing off, canonical off, no extended functions,
    // no signal chars (^Z,^C)
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    
    // Control chars: set return condition: min number of bytes and timer
    raw.c_cc[VMIN] = 1;   // Read at least one character before returning
    raw.c_cc[VTIME] = 0;  // No timeout
    
    // Apply the new settings
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0) {
        ERROR_SYSTEM(ERR_SYSTEM, "Failed to set terminal attributes");
        return;
    }
    
    raw_mode_enabled = 1;
}

/**
 * @brief Disable raw mode
 * 
 * Restores the terminal to its original mode.
 */
void disable_raw_mode(void) {
    if (!raw_mode_enabled) {
        return;  // Not in raw mode
    }
    
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) < 0) {
        ERROR_SYSTEM(ERR_SYSTEM, "Failed to restore terminal attributes");
    }
    
    raw_mode_enabled = 0;
}

/**
 * @brief Read a character from terminal
 * 
 * Reads a single character from the terminal.
 * 
 * @return int Character read, or -1 on error
 */
int read_char(void) {
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) {
        return -1;
    }
    return (int)c;
}

/**
 * @brief Read a line from terminal
 * 
 * Reads a line of input with editing capabilities.
 * 
 * @param buffer Buffer to store the input
 * @param size Buffer size
 * @return int Number of characters read, or -1 on error
 */
int read_line(char *buffer, int size) {
    if (!buffer || size <= 0) {
        return -1;
    }
    
    LineState ls;
    
    // Initialize line state
    if (line_state_init(&ls, size) != 0) {
        return -1;
    }
    
    // Enable raw mode for reading
    int orig_raw_mode = raw_mode_enabled;
    if (!raw_mode_enabled) {
        enable_raw_mode();
    }
    
    // Read edited line
    int result = read_edited_line(&ls);
    
    // Copy the result to the buffer
    if (result >= 0) {
        strncpy(buffer, ls.buffer, size - 1);
        buffer[size - 1] = '\0';  // Ensure null termination
    }
    
    // Clean up line state
    line_state_cleanup(&ls);
    
    // Restore terminal mode if it wasn't in raw mode before
    if (!orig_raw_mode) {
        disable_raw_mode();
    }
    
    return result;
}

/**
 * @brief Clear the screen
 * 
 * Clears the terminal screen.
 */
void clear_screen(void) {
    // Use ANSI escape codes to clear screen and move cursor to home position
    printf("\033[2J\033[H");
    fflush(stdout);
}

/**
 * @brief Display shell prompt
 * 
 * Displays the shell prompt.
 */
void display_prompt(void) {
    // Get current working directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        strcpy(cwd, "unknown");
    }
    
    // Display prompt with current directory
    printf("\033[1;32m%s\033[0m$ ", cwd);
    fflush(stdout);
}

/**
 * @brief Handle window resize
 * 
 * Adjusts terminal state after window resize.
 */
void handle_resize(void) {
    // Get new terminal size
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        ERROR_SYSTEM(ERR_SYSTEM, "Failed to get terminal size");
        return;
    }
    
    // Redraw prompt and current input line
    // This would be called from a SIGWINCH handler
    // For now, just redisplay the prompt
    printf("\r");
    display_prompt();
}

/**
 * @brief Get terminal size
 * 
 * Gets the current terminal dimensions.
 * 
 * @param rows Pointer to store row count
 * @param cols Pointer to store column count
 * @return int 0 on success, non-zero on error
 */
int get_terminal_size(int *rows, int *cols) {
    struct winsize ws;
    
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        ERROR_SYSTEM(ERR_SYSTEM, "Failed to get terminal size");
        return -1;
    }
    
    *rows = ws.ws_row;
    *cols = ws.ws_col;
    
    return 0;
}

/**
 * @brief Refresh the current line
 * 
 * Redraws the current input line.
 * 
 * @param buffer Current input buffer
 * @param cursor Cursor position
 */
void refresh_line(const char *buffer, int cursor) {
    // Go to the beginning of the line
    printf("\r");
    
    // Print the prompt
    display_prompt();
    
    // Print the buffer
    printf("%s", buffer);
    
    // Calculate cursor position
    int prompt_len = 2; // $ plus space
    int pos = prompt_len + cursor;
    
    // Move cursor to the correct position
    printf("\r\033[%dC", pos);
    
    fflush(stdout);
}

/**
 * @brief Set input processing mode
 * 
 * Sets terminal input processing mode.
 * 
 * @param original_termios Pointer to store original termios 
 * @param raw 1 for raw mode, 0 for canonical mode
 * @return int 0 on success, non-zero on error
 */
int set_input_mode(struct termios *original_termios, int raw) {
    if (!original_termios) {
        return -1;
    }
    
    if (raw) {
        // Enable raw mode
        struct termios raw = *original_termios;
        raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        raw.c_cflag |= (CS8);
        raw.c_oflag &= ~(OPOST);
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;
        
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0) {
            ERROR_SYSTEM(ERR_SYSTEM, "Failed to set terminal attributes");
            return -1;
        }
    } else {
        // Restore original mode
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, original_termios) < 0) {
            ERROR_SYSTEM(ERR_SYSTEM, "Failed to restore terminal attributes");
            return -1;
        }
    }
    
    return 0;
}