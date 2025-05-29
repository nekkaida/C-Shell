/**
 * @file input.c
 * @brief Input handling implementation
 *
 * Implementation of input handling functions.
 */

#include "terminal/input.h"
#include "terminal/terminal.h"
#include "completion/completion.h"
#include "utils/error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>

/**
 * @brief Initialize line state
 * 
 * @param ls LineState structure to initialize
 * @param capacity Initial buffer capacity
 * @return int 0 on success, non-zero on error
 */
int line_state_init(LineState *ls, int capacity) {
    if (!ls || capacity <= 0) {
        return -1;
    }
    
    ls->buffer = (char *)malloc(capacity);
    if (!ls->buffer) {
        ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
        return -1;
    }
    
    ls->buffer[0] = '\0';
    ls->length = 0;
    ls->capacity = capacity;
    ls->cursor = 0;
    
    return 0;
}

/**
 * @brief Clean up line state
 * 
 * @param ls LineState structure to clean up
 */
void line_state_cleanup(LineState *ls) {
    if (ls && ls->buffer) {
        free(ls->buffer);
        ls->buffer = NULL;
        ls->length = 0;
        ls->capacity = 0;
        ls->cursor = 0;
    }
}

/**
 * @brief Read a line with editing capabilities
 * 
 * @param ls LineState structure
 * @return int Characters read or -1 on error
 */
int read_edited_line(LineState *ls) {
    if (!ls || !ls->buffer) {
        return -1;
    }
    
    int c;
    
    // Loop until we get a newline or error
    while (1) {
        c = read_char();
        
        if (c == -1) {
            return -1;
        }
        
        // Process the character
        if (c == '\n' || c == '\r') {
            // Enter key - finish line
            printf("\n");
            break;
        } else if (c == KEY_CTRL_C) {
            // Ctrl+C - abort
            printf("^C\n");
            ls->buffer[0] = '\0';
            ls->length = 0;
            ls->cursor = 0;
            return 0;
        } else if (c == KEY_CTRL_D) {
            // Ctrl+D - EOF (only if buffer is empty)
            if (ls->length == 0) {
                return -1;
            }
        } else if (c == KEY_BACKSPACE || c == KEY_CTRL_H) {
            // Backspace
            if (ls->cursor > 0) {
                delete_char(ls);
                refresh_line(ls->buffer, ls->cursor);
            }
        } else if (c == KEY_TAB) {
            // Tab completion
            handle_tab_completion(ls->buffer, &ls->cursor);
            ls->length = strlen(ls->buffer);
            refresh_line(ls->buffer, ls->cursor);
        } else if (c == KEY_ESC) {
            // Escape sequence
            int seq = read_escape_sequence();
            
            if (seq == SEQ_LEFT) {
                // Left arrow
                move_cursor_left(ls);
                refresh_line(ls->buffer, ls->cursor);
            } else if (seq == SEQ_RIGHT) {
                // Right arrow
                move_cursor_right(ls);
                refresh_line(ls->buffer, ls->cursor);
            } else if (seq == SEQ_HOME) {
                // Home key
                move_cursor_home(ls);
                refresh_line(ls->buffer, ls->cursor);
            } else if (seq == SEQ_END) {
                // End key
                move_cursor_end(ls);
                refresh_line(ls->buffer, ls->cursor);
            } else if (seq == SEQ_DELETE) {
                // Delete key
                if (ls->cursor < ls->length) {
                    // Move cursor right and then backspace
                    move_cursor_right(ls);
                    delete_char(ls);
                    refresh_line(ls->buffer, ls->cursor);
                }
            }
        } else if (c == KEY_CTRL_A) {
            // Ctrl+A - Go to beginning of line
            move_cursor_home(ls);
            refresh_line(ls->buffer, ls->cursor);
        } else if (c == KEY_CTRL_E) {
            // Ctrl+E - Go to end of line
            move_cursor_end(ls);
            refresh_line(ls->buffer, ls->cursor);
        } else if (c == KEY_CTRL_K) {
            // Ctrl+K - Kill to end of line
            kill_to_end(ls);
            refresh_line(ls->buffer, ls->cursor);
        } else if (c == KEY_CTRL_U) {
            // Ctrl+U - Kill to beginning of line
            kill_to_start(ls);
            refresh_line(ls->buffer, ls->cursor);
        } else if (c == KEY_CTRL_W) {
            // Ctrl+W - Kill previous word
            kill_prev_word(ls);
            refresh_line(ls->buffer, ls->cursor);
        } else if (c == KEY_CTRL_L) {
            // Ctrl+L - Clear screen
            clear_screen();
            refresh_line(ls->buffer, ls->cursor);
        } else if (isprint(c)) {
            // Printable character - insert
            insert_char(ls, c);
            refresh_line(ls->buffer, ls->cursor);
        }
    }
    
    return ls->length;
}

/**
 * @brief Insert character at cursor
 * 
 * @param ls LineState structure
 * @param c Character to insert
 * @return int 0 on success, non-zero on error
 */
int insert_char(LineState *ls, char c) {
    if (!ls || !ls->buffer) {
        return -1;
    }
    
    // Make sure we have enough space
    if (ls->length + 1 >= ls->capacity) {
        // Resize buffer
        int new_capacity = ls->capacity * 2;
        char *new_buffer = realloc(ls->buffer, new_capacity);
        if (!new_buffer) {
            ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
            return -1;
        }
        
        ls->buffer = new_buffer;
        ls->capacity = new_capacity;
    }
    
    // Make room for the new character
    if (ls->cursor < ls->length) {
        // Move characters after cursor
        memmove(ls->buffer + ls->cursor + 1, ls->buffer + ls->cursor, ls->length - ls->cursor);
    }
    
    // Insert character
    ls->buffer[ls->cursor] = c;
    ls->cursor++;
    ls->length++;
    ls->buffer[ls->length] = '\0';
    
    return 0;
}

/**
 * @brief Delete character at cursor
 * 
 * @param ls LineState structure
 * @return int 0 on success, non-zero on error
 */
int delete_char(LineState *ls) {
    if (!ls || !ls->buffer || ls->cursor <= 0) {
        return -1;
    }
    
    // Move cursor back
    ls->cursor--;
    
    // Remove character
    if (ls->cursor < ls->length - 1) {
        // Move characters after cursor
        memmove(ls->buffer + ls->cursor, ls->buffer + ls->cursor + 1, ls->length - ls->cursor - 1);
    }
    
    ls->length--;
    ls->buffer[ls->length] = '\0';
    
    return 0;
}

/**
 * @brief Move cursor left
 * 
 * @param ls LineState structure
 * @return int 0 on success, non-zero on error
 */
int move_cursor_left(LineState *ls) {
    if (!ls || ls->cursor <= 0) {
        return -1;
    }
    
    ls->cursor--;
    return 0;
}

/**
 * @brief Move cursor right
 * 
 * @param ls LineState structure
 * @return int 0 on success, non-zero on error
 */
int move_cursor_right(LineState *ls) {
    if (!ls || ls->cursor >= ls->length) {
        return -1;
    }
    
    ls->cursor++;
    return 0;
}

/**
 * @brief Move cursor to start of line
 * 
 * @param ls LineState structure
 * @return int 0 on success, non-zero on error
 */
int move_cursor_home(LineState *ls) {
    if (!ls) {
        return -1;
    }
    
    ls->cursor = 0;
    return 0;
}

/**
 * @brief Move cursor to end of line
 * 
 * @param ls LineState structure
 * @return int 0 on success, non-zero on error
 */
int move_cursor_end(LineState *ls) {
    if (!ls) {
        return -1;
    }
    
    ls->cursor = ls->length;
    return 0;
}

/**
 * @brief Delete to end of line
 * 
 * @param ls LineState structure
 * @return int 0 on success, non-zero on error
 */
int kill_to_end(LineState *ls) {
    if (!ls || !ls->buffer) {
        return -1;
    }
    
    // Truncate line at cursor
    ls->length = ls->cursor;
    ls->buffer[ls->length] = '\0';
    
    return 0;
}

/**
 * @brief Delete to start of line
 * 
 * @param ls LineState structure
 * @return int 0 on success, non-zero on error
 */
int kill_to_start(LineState *ls) {
    if (!ls || !ls->buffer || ls->cursor <= 0) {
        return -1;
    }
    
    // Move text after cursor to beginning of line
    if (ls->cursor < ls->length) {
        memmove(ls->buffer, ls->buffer + ls->cursor, ls->length - ls->cursor);
    }
    
    ls->length -= ls->cursor;
    ls->buffer[ls->length] = '\0';
    ls->cursor = 0;
    
    return 0;
}

/**
 * @brief Delete previous word
 * 
 * @param ls LineState structure
 * @return int 0 on success, non-zero on error
 */
int kill_prev_word(LineState *ls) {
    if (!ls || !ls->buffer || ls->cursor <= 0) {
        return -1;
    }
    
    // Find start of previous word
    int old_cursor = ls->cursor;
    int new_cursor = ls->cursor - 1;
    
    // Skip whitespace
    while (new_cursor > 0 && isspace(ls->buffer[new_cursor])) {
        new_cursor--;
    }
    
    // Skip word
    while (new_cursor > 0 && !isspace(ls->buffer[new_cursor - 1])) {
        new_cursor--;
    }
    
    // Move text after cursor to new position
    if (old_cursor < ls->length) {
        memmove(ls->buffer + new_cursor, ls->buffer + old_cursor, ls->length - old_cursor);
    }
    
    ls->length -= (old_cursor - new_cursor);
    ls->buffer[ls->length] = '\0';
    ls->cursor = new_cursor;
    
    return 0;
}

/**
 * @brief Process special key
 * 
 * @param ls LineState structure
 * @param key Key code
 * @return int 0 on success, non-zero on error
 */
int process_key(LineState *ls, int key) {
    if (!ls) {
        return -1;
    }
    
    switch (key) {
        case KEY_CTRL_A:  // Home
            return move_cursor_home(ls);
        
        case KEY_CTRL_B:  // Left
        case KEY_ESC:     // Escape (assuming left arrow)
            return move_cursor_left(ls);
        
        case KEY_CTRL_D:  // Delete
            if (ls->length == 0) {
                return -1;  // EOF
            }
            // Fall through to right then backspace
            
        case KEY_CTRL_F:  // Right
            return move_cursor_right(ls);
        
        case KEY_CTRL_E:  // End
            return move_cursor_end(ls);
        
        case KEY_CTRL_H:  // Backspace
        case KEY_BACKSPACE:
            return delete_char(ls);
        
        case KEY_CTRL_K:  // Kill to end
            return kill_to_end(ls);
        
        case KEY_CTRL_U:  // Kill to beginning
            return kill_to_start(ls);
        
        case KEY_CTRL_W:  // Kill word
            return kill_prev_word(ls);
        
        default:
            return -1;
    }
}

/**
 * @brief Read escape sequence
 * 
 * @return int Escape sequence code
 */
int read_escape_sequence(void) {
    // Read the next character
    int c = read_char();
    
    if (c == -1) {
        return KEY_ESC;  // Just an escape key
    }
    
    if (c == '[') {
        // CSI sequence
        c = read_char();
        
        if (c == -1) {
            return KEY_ESC;  // Incomplete sequence
        }
        
        if (c >= '0' && c <= '9') {
            // Extended sequence
            int code = c - '0';
            c = read_char();
            
            if (c == '~') {
                // Simple extended sequence
                switch (code) {
                    case 1: return SEQ_HOME;
                    case 3: return SEQ_DELETE;
                    case 4: return SEQ_END;
                    case 5: return SEQ_PAGE_UP;
                    case 6: return SEQ_PAGE_DOWN;
                    default: return KEY_ESC;
                }
            } else if (c >= '0' && c <= '9') {
                // Two-digit code
                code = code * 10 + (c - '0');
                c = read_char();
                
                if (c == '~') {
                    switch (code) {
                        case 15: return SEQ_HOME;
                        case 17: return SEQ_END;
                        default: return KEY_ESC;
                    }
                }
            }
        } else {
            // Simple sequence
            switch (c) {
                case 'A': return SEQ_UP;
                case 'B': return SEQ_DOWN;
                case 'C': return SEQ_RIGHT;
                case 'D': return SEQ_LEFT;
                case 'H': return SEQ_HOME;
                case 'F': return SEQ_END;
                default: return KEY_ESC;
            }
        }
    } else if (c == 'O') {
        // SS3 sequence
        c = read_char();
        
        switch (c) {
            case 'A': return SEQ_UP;
            case 'B': return SEQ_DOWN;
            case 'C': return SEQ_RIGHT;
            case 'D': return SEQ_LEFT;
            case 'H': return SEQ_HOME;
            case 'F': return SEQ_END;
            default: return KEY_ESC;
        }
    }
    
    return KEY_ESC;  // Unknown sequence
}