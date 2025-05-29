/**
 * @file error.h
 * @brief Error handling utilities
 *
 * Functions for error handling and reporting.
 */

#ifndef ERROR_H
#define ERROR_H

#include <stdarg.h>

/**
 * @brief Error levels
 */
typedef enum {
    ERROR_DEBUG,    /**< Debug message (only shown in verbose mode) */
    ERROR_INFO,     /**< Informational message */
    ERROR_WARNING,  /**< Warning message */
    ERROR_ERROR,    /**< Error message */
    ERROR_FATAL     /**< Fatal error (terminates program) */
} ErrorLevel;

/**
 * @brief Error codes
 */
typedef enum {
    ERR_NONE = 0,           /**< No error */
    ERR_MEMORY = -1,        /**< Memory allocation error */
    ERR_IO = -2,            /**< I/O error */
    ERR_PARSE = -3,         /**< Parsing error */
    ERR_SYNTAX = -4,        /**< Syntax error */
    ERR_NOT_FOUND = -5,     /**< Entity not found */
    ERR_ACCESS = -6,        /**< Access denied */
    ERR_INTERNAL = -7,      /**< Internal error */
    ERR_SYSTEM = -8,        /**< System call error */
    ERR_INVALID_ARG = -9,   /**< Invalid argument */
    ERR_NOT_IMPLEMENTED = -10 /**< Feature not implemented */
} ErrorCode;

/**
 * @brief Set error level
 * 
 * Sets the minimum error level to display.
 * 
 * @param level Minimum error level
 */
void error_set_level(ErrorLevel level);

/**
 * @brief Enable/disable verbose mode
 * 
 * @param verbose 1 to enable, 0 to disable
 */
void error_set_verbose(int verbose);

/**
 * @brief Report an error
 * 
 * Reports an error message.
 * 
 * @param level Error level
 * @param code Error code
 * @param file Source file
 * @param line Line number
 * @param func Function name
 * @param format Format string
 * @param ... Additional arguments
 */
void error_report(ErrorLevel level, ErrorCode code, const char *file, 
                  int line, const char *func, const char *format, ...);

/**
 * @brief Report a system error
 * 
 * Reports an error message with errno information.
 * 
 * @param level Error level
 * @param code Error code
 * @param file Source file
 * @param line Line number
 * @param func Function name
 * @param format Format string
 * @param ... Additional arguments
 */
void error_system(ErrorLevel level, ErrorCode code, const char *file, 
                 int line, const char *func, const char *format, ...);

/**
 * @brief Report a fatal error and exit
 * 
 * Reports a fatal error message and exits the program.
 * 
 * @param code Error code
 * @param file Source file
 * @param line Line number
 * @param func Function name
 * @param format Format string
 * @param ... Additional arguments
 */
void error_fatal(ErrorCode code, const char *file, int line, 
                const char *func, const char *format, ...);

/**
 * @brief Report a debug message
 * 
 * Reports a debug message (only shown in verbose mode).
 * 
 * @param file Source file
 * @param line Line number
 * @param func Function name
 * @param format Format string
 * @param ... Additional arguments
 */
void error_debug(const char *file, int line, const char *func, 
                const char *format, ...);

/**
 * @brief Get error message for code
 * 
 * Gets a string description for an error code.
 * 
 * @param code Error code
 * @return const char* Error message
 */
const char *error_message(ErrorCode code);

/**
 * @brief Set error handler
 * 
 * Sets a custom error handler function.
 * 
 * @param handler Error handler function
 */
void error_set_handler(void (*handler)(ErrorLevel level, ErrorCode code, 
                                      const char *file, int line, 
                                      const char *func, const char *msg));

/**
 * @brief Convenience macros for error reporting
 */
#define ERROR_DEBUG(fmt, ...) \
    error_debug(__FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define ERROR_INFO(code, fmt, ...) \
    error_report(ERROR_INFO, code, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define ERROR_WARNING(code, fmt, ...) \
    error_report(ERROR_WARNING, code, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define ERROR_ERROR(code, fmt, ...) \
    error_report(ERROR_ERROR, code, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define ERROR_FATAL(code, fmt, ...) \
    error_fatal(code, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define ERROR_SYSTEM(code, fmt, ...) \
    error_system(ERROR_ERROR, code, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#endif /* ERROR_H */