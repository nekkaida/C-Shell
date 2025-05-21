/**
 * @file input.h
 * @brief Input handling component
 *
 * Functions for handling user input and line editing.
 */

#ifndef INPUT_H
#define INPUT_H

/**
 * @brief Special key codes
 */
enum KeyCode {
    KEY_NULL = 0,      /**< Null byte */
    KEY_CTRL_A = 1,    /**< Ctrl+A (Home) */
    KEY_CTRL_B = 2,    /**< Ctrl+B (Left) */
    KEY_CTRL_C = 3,    /**< Ctrl+C (Interrupt) */
    KEY_CTRL_D = 4,    /**< Ctrl+D (EOF) */
    KEY_CTRL_E = 5,    /**< Ctrl+E (End) */
    KEY_CTRL_F = 6,    /**< Ctrl+F (Right) */
    KEY_CTRL_H = 8,    /**< Ctrl+H (Backspace) */
    KEY_TAB = 9,       /**< Tab */
    KEY_CTRL_K = 11,   /**< Ctrl+K (Kill to end) */
    KEY_CTRL_L = 12,   /**< Ctrl+L (Clear screen) */
    KEY_ENTER = 13,    /**< Enter */
    KEY_CTRL_N = 14,   /**< Ctrl+N (Next history) */
    KEY_CTRL_P = 16,   /**< Ctrl+P (Previous history) */
    KEY_CTRL_T = 20,   /**< Ctrl+T (Transpose chars) */
    KEY_CTRL_U = 21,   /**< Ctrl+U (Kill to beginning) */
    KEY_CTRL_W = 23,   /**< Ctrl+W (Kill word) */
    KEY_ESC = 27,      /**< Escape sequence */
    KEY_BACKSPACE = 127 /**< Backspace */
};

/**
 * @brief Escape sequence codes
 */
enum EscapeSequence {
    SEQ_UP = 1000,     /**< Up arrow */
    SEQ_DOWN,          /**< Down arrow */
    SEQ_RIGHT,         /**< Right arrow */
    SEQ_LEFT,          /**< Left arrow */
    SEQ_HOME,          /**< Home key */
    SEQ_END,           /**< End key */
    SEQ_DELETE,        /**< Delete key */
    SEQ_PAGE_UP,       /**< Page Up */
    SEQ_PAGE_DOWN      /**< Page Down */
};

/**
 * @brief Line editing state
 */
typedef struct {
    char *buffer;      /**< Input buffer */
    int length;        /**< Current length */
    int capacity;      /**< Buffer capacity */
    int cursor;        /**< Cursor position */
} LineState;

/**
 * @brief Initialize line state
 * 
 * @param ls LineState structure to initialize
 * @param capacity Initial buffer capacity
 * @return int 0 on success, non-zero on error
 */
int line_state_init(LineState *ls, int capacity);

/**
 * @brief Clean up line state
 * 
 * @param ls LineState structure to clean up
 */
void line_state_cleanup(LineState *ls);

/**
 * @brief Read a line with editing capabilities
 * 
 * @param ls LineState structure
 * @return int Characters read or -1 on error
 */
int read_edited_line(LineState *ls);

/**
 * @brief Insert character at cursor
 * 
 * @param ls LineState structure
 * @param c Character to insert
 * @return int 0 on success, non-zero on error
 */
int insert_char(LineState *ls, char c);

/**
 * @brief Delete character at cursor
 * 
 * @param ls LineState structure
 * @return int 0 on success, non-zero on error
 */
int delete_char(LineState *ls);

/**
 * @brief Move cursor left
 * 
 * @param ls LineState structure
 * @return int 0 on success, non-zero on error
 */
int move_cursor_left(LineState *ls);

/**
 * @brief Move cursor right
 * 
 * @param ls LineState structure
 * @return int 0 on success, non-zero on error
 */
int move_cursor_right(LineState *ls);

/**
 * @brief Move cursor to start of line
 * 
 * @param ls LineState structure
 * @return int 0 on success, non-zero on error
 */
int move_cursor_home(LineState *ls);

/**
 * @brief Move cursor to end of line
 * 
 * @param ls LineState structure
 * @return int 0 on success, non-zero on error
 */
int move_cursor_end(LineState *ls);

/**
 * @brief Delete to end of line
 * 
 * @param ls LineState structure
 * @return int 0 on success, non-zero on error
 */
int kill_to_end(LineState *ls);

/**
 * @brief Delete to start of line
 * 
 * @param ls LineState structure
 * @return int 0 on success, non-zero on error
 */
int kill_to_start(LineState *ls);

/**
 * @brief Delete previous word
 * 
 * @param ls LineState structure
 * @return int 0 on success, non-zero on error
 */
int kill_prev_word(LineState *ls);

/**
 * @brief Process special key
 * 
 * @param ls LineState structure
 * @param key Key code
 * @return int 0 on success, non-zero on error
 */
int process_key(LineState *ls, int key);

/**
 * @brief Read escape sequence
 * 
 * @return int Escape sequence code
 */
int read_escape_sequence(void);

#endif /* INPUT_H */