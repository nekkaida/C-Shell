/**
 * @file terminal.h
 * @brief Terminal handling component
 *
 * Functions for terminal management and input processing.
 */

#ifndef TERMINAL_H
#define TERMINAL_H

#include <termios.h>

/**
 * @brief Initialize terminal settings
 * 
 * Sets up the terminal for shell operation.
 * 
 * @return int 0 on success, non-zero on error
 */
int terminal_init(void);

/**
 * @brief Clean up terminal settings
 * 
 * Restores original terminal settings.
 */
void terminal_cleanup(void);

/**
 * @brief Enable raw mode
 * 
 * Puts the terminal in raw mode for character-by-character input.
 */
void enable_raw_mode(void);

/**
 * @brief Disable raw mode
 * 
 * Restores the terminal to its original mode.
 */
void disable_raw_mode(void);

/**
 * @brief Read a character from terminal
 * 
 * Reads a single character from the terminal.
 * 
 * @return int Character read, or -1 on error
 */
int read_char(void);

/**
 * @brief Read a line from terminal
 * 
 * Reads a line of input with editing capabilities.
 * 
 * @param buffer Buffer to store the input
 * @param size Buffer size
 * @return int Number of characters read, or -1 on error
 */
int read_line(char *buffer, int size);

/**
 * @brief Clear the screen
 * 
 * Clears the terminal screen.
 */
void clear_screen(void);

/**
 * @brief Display shell prompt
 * 
 * Displays the shell prompt.
 */
void display_prompt(void);

/**
 * @brief Handle window resize
 * 
 * Adjusts terminal state after window resize.
 */
void handle_resize(void);

/**
 * @brief Get terminal size
 * 
 * Gets the current terminal dimensions.
 * 
 * @param rows Pointer to store row count
 * @param cols Pointer to store column count
 * @return int 0 on success, non-zero on error
 */
int get_terminal_size(int *rows, int *cols);

/**
 * @brief Refresh the current line
 * 
 * Redraws the current input line.
 * 
 * @param buffer Current input buffer
 * @param cursor Cursor position
 */
void refresh_line(const char *buffer, int cursor);

/**
 * @brief Set input processing mode
 * 
 * Sets terminal input processing mode.
 * 
 * @param original_termios Pointer to store original termios 
 * @param raw 1 for raw mode, 0 for canonical mode
 * @return int 0 on success, non-zero on error
 */
int set_input_mode(struct termios *original_termios, int raw);

#endif /* TERMINAL_H */