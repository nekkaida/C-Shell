/**
 * @file error.c
 * @brief Error handling implementation
 *
 * Implementation of error handling functions.
 */

#include "utils/error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

/** Current error level */
static ErrorLevel current_error_level = ERROR_WARNING;

/** Custom error handler */
static void (*error_handler)(ErrorLevel level, ErrorCode code, 
                            const char *file, int line, 
                            const char *func, const char *msg) = NULL;

/** Verbose mode flag */
static int verbose_mode = 0;

/**
 * @brief Error level strings
 */
static const char *error_level_strings[] = {
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR",
    "FATAL"
};

/**
 * @brief Error code messages
 */
static const char *error_code_messages[] = {
    "No error",                  // ERR_NONE
    "Memory allocation error",   // ERR_MEMORY
    "I/O error",                 // ERR_IO
    "Parsing error",             // ERR_PARSE
    "Syntax error",              // ERR_SYNTAX
    "Entity not found",          // ERR_NOT_FOUND
    "Access denied",             // ERR_ACCESS
    "Internal error",            // ERR_INTERNAL
    "System call error",         // ERR_SYSTEM
    "Invalid argument",          // ERR_INVALID_ARG
    "Feature not implemented"    // ERR_NOT_IMPLEMENTED
};

/**
 * @brief Set error level
 * 
 * Sets the minimum error level to display.
 * 
 * @param level Minimum error level
 */
void error_set_level(ErrorLevel level) {
    current_error_level = level;
}

/**
 * @brief Enable/disable verbose mode
 * 
 * @param verbose 1 to enable, 0 to disable
 */
void error_set_verbose(int verbose) {
    verbose_mode = verbose;
}

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
                  int line, const char *func, const char *format, ...) {
    // Check if error level is high enough to be reported
    if (level < current_error_level) {
        return;
    }
    
    // Don't show debug messages unless in verbose mode
    if (level == ERROR_DEBUG && !verbose_mode) {
        return;
    }
    
    // Format the error message
    char message[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    // Call custom error handler if set
    if (error_handler) {
        error_handler(level, code, file, line, func, message);
        return;
    }
    
    // Standard error handling
    const char *level_str = error_level_strings[level];
    
    if (level == ERROR_DEBUG) {
        // Debug message with file/line/function
        fprintf(stderr, "[%s] %s:%d (%s): %s\n", 
                level_str, file, line, func, message);
    } else {
        // Regular error message
        fprintf(stderr, "[%s] %s\n", level_str, message);
    }
    
    // Exit on fatal error
    if (level == ERROR_FATAL) {
        exit(code < 0 ? -code : 1);
    }
}

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
                 int line, const char *func, const char *format, ...) {
    // Check if error level is high enough to be reported
    if (level < current_error_level) {
        return;
    }
    
    // Format the error message
    char message[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    // Append system error message
    int saved_errno = errno;
    char full_message[2048];
    snprintf(full_message, sizeof(full_message), "%s: %s", 
             message, strerror(saved_errno));
    
    // Report the error
    error_report(level, code, file, line, func, full_message);
}

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
                const char *func, const char *format, ...) {
    // Format the error message
    char message[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    // Report the error and exit
    error_report(ERROR_FATAL, code, file, line, func, message);
    
    // Should never get here, but just in case
    exit(code < 0 ? -code : 1);
}

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
                const char *format, ...) {
    // Check if in verbose mode
    if (!verbose_mode) {
        return;
    }
    
    // Format the debug message
    char message[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    // Report the debug message
    error_report(ERROR_DEBUG, ERR_NONE, file, line, func, message);
}

/**
 * @brief Get error message for code
 * 
 * Gets a string description for an error code.
 * 
 * @param code Error code
 * @return const char* Error message
 */
const char *error_message(ErrorCode code) {
    int index = -code;
    
    if (index < 0 || index >= (int)(sizeof(error_code_messages) / sizeof(error_code_messages[0]))) {
        return "Unknown error code";
    }
    
    return error_code_messages[index];
}

/**
 * @brief Set error handler
 * 
 * Sets a custom error handler function.
 * 
 * @param handler Error handler function
 */
void error_set_handler(void (*handler)(ErrorLevel level, ErrorCode code, 
                                      const char *file, int line, 
                                      const char *func, const char *msg)) {
    error_handler = handler;
}