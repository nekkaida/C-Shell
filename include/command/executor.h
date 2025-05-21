/**
 * @file parser.h
 * @brief Command parsing component
 *
 * Functions for parsing command input into arguments and redirection operators.
 */

#ifndef PARSER_H
#define PARSER_H

#include "command.h"

/**
 * @brief Parse a command line into tokens
 * 
 * Parses a command line into tokens, respecting quotes and escape characters.
 * 
 * @param line Input command line
 * @param tokens Array to store tokens (must be preallocated)
 * @param max_tokens Maximum number of tokens to parse
 * @return int Number of tokens parsed
 */
int tokenize_command(const char *line, char **tokens, int max_tokens);

/**
 * @brief Parse echo command arguments
 * 
 * Special handling for echo command arguments with escape sequences.
 * 
 * @param input Input arguments string
 * @param output Output buffer for processed arguments
 * @param output_size Size of output buffer
 */
void parse_echo_args(const char *input, char *output, size_t output_size);

/**
 * @brief Process escape sequences
 * 
 * Processes escape sequences in a string.
 * 
 * @param input Input string with escape sequences
 * @param output Output buffer for processed string
 * @param output_size Size of output buffer
 * @return size_t Number of bytes written to output
 */
size_t process_escapes(const char *input, char *output, size_t output_size);

/**
 * @brief Handle quotes in a string
 * 
 * Removes quotes and handles quoted content.
 * 
 * @param input Input string with quotes
 * @param output Output buffer for processed string
 * @param output_size Size of output buffer
 * @return size_t Number of bytes written to output
 */
size_t process_quotes(const char *input, char *output, size_t output_size);

/**
 * @brief Validates command syntax
 * 
 * Checks for syntax errors in a command string.
 * 
 * @param input Command string to validate
 * @return int 0 if syntax is valid, error code otherwise
 */
int validate_command_syntax(const char *input);

#endif /* PARSER_H */