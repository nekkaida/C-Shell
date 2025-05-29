/**
 * @file parser.c
 * @brief Command parsing implementation
 *
 * Implementation of command parsing functions.
 */

#include "command/parser.h"
#include "command/command.h"
#include "utils/error.h"
#include "utils/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Parse redirections from input string
 * 
 * Extracts redirection operators from a command string.
 * 
 * @param input Command string (will be modified)
 * @param redir RedirectionInfo structure to fill
 * @return int 0 on success, non-zero on error
 */
int parse_redirections(char *input, RedirectionInfo *redir) {
    if (!input || !redir) return -1;

    // Variables to track quote state
    int in_single_quotes = 0;
    int in_double_quotes = 0;
    
    // Find positions of redirection operators while respecting quotes
    char *redirect_stdout_pos = NULL;
    char *redirect_1_pos = NULL;
    char *redirect_2_pos = NULL;
    
    char *current = input;
    while (*current != '\0') {
        if (*current == '\\' && !in_single_quotes) {
            // Skip the escaped character
            current++;
            if (*current != '\0') current++;
            continue;
        } else if (*current == '\'' && !in_double_quotes) {
            in_single_quotes = !in_single_quotes;
        } else if (*current == '"' && !in_single_quotes) {
            in_double_quotes = !in_double_quotes;
        } else if (!in_single_quotes && !in_double_quotes) {
            if (*current == '>' && current > input) {
                // Check for ">>" (append) vs ">" (overwrite)
                int is_append = (*(current+1) == '>');
                
                if (*(current-1) == '1') {
                    redirect_1_pos = current - 1;
                    if (is_append) {
                        redir->append_stdout = 1;
                        current++; // Skip the second '>'
                    }
                } else if (*(current-1) == '2') {
                    redirect_2_pos = current - 1;
                    if (is_append) {
                        redir->append_stderr = 1;
                        current++; // Skip the second '>'
                    }
                } else {
                    redirect_stdout_pos = current;
                    if (is_append) {
                        redir->append_stdout = 1;
                        current++; // Skip the second '>'
                    }
                }
            }
        }
        current++;
    }
    
    // Check for unclosed quotes
    if (in_single_quotes || in_double_quotes) {
        ERROR_ERROR(ERR_SYNTAX, "Unclosed quotes in command");
        return -1;
    }
    
    // Process stdout redirection
    if (redirect_stdout_pos != NULL || redirect_1_pos != NULL) {
        char *redirect_pos = redirect_stdout_pos != NULL ? redirect_stdout_pos : redirect_1_pos;
        int is_1_redirect = (redirect_1_pos != NULL);
        
        // Terminate the command at the redirection operator
        *redirect_pos = '\0';
        if (is_1_redirect) {
            *(redirect_pos - 1) = '\0';  // Remove the '1' as well
        }
        
        // Extract the filename
        char *filename = redirect_pos + 1;
        if (is_1_redirect) {
            filename++;  // Skip "1>"
        }
        
        // Skip an additional character if appending (for the second '>')
        if (redir->append_stdout) {
            filename++;
        }
        
        // Skip spaces
        while (*filename == ' ') {
            filename++;
        }
        
        // Find the end of the filename (stop at the next redirection or end of string)
        char *filename_end = filename;
        while (*filename_end != '\0' && *filename_end != '>') {
            filename_end++;
        }
        
        // Terminate the filename if another redirection was found
        if (*filename_end == '>') {
            char *temp = filename_end;
            // Go back to skip any spaces
            while (temp > filename && *(temp-1) == ' ') {
                temp--;
            }
            *temp = '\0';
        } else {
            // Remove trailing spaces
            char *end = filename_end - 1;
            while (end >= filename && *end == ' ') {
                *end = '\0';
                end--;
            }
        }

        // Store the filename
        redir->stdout_file = strdup(filename);
        if (!redir->stdout_file) {
            ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
            return -1;
        }
        
        // Trim any excess whitespace
        str_trim(redir->stdout_file);
    }
    
    // Process stderr redirection
    if (redirect_2_pos != NULL) {
        // Find '2>' or '2>>' in the command
        char *p = input;
        char *redirect_pos = NULL;
        
        while (*p != '\0') {
            if (*p == '2' && *(p+1) == '>') {
                redirect_pos = p;
                break;
            }
            p++;
        }
        
        if (redirect_pos != NULL) {
            // Terminate the command at the redirection operator
            *redirect_pos = '\0';
            
            // Extract the filename
            char *filename = redirect_pos + 2;  // Skip "2>"
            
            // Skip an additional character if appending (for the second '>')
            if (redir->append_stderr) {
                filename++;
            }
            
            // Skip spaces
            while (*filename == ' ') {
                filename++;
            }
            
            // Remove trailing spaces
            char *end = filename + strlen(filename) - 1;
            while (end >= filename && *end == ' ') {
                *end = '\0';
                end--;
            }
            
            // Store the filename
            redir->stderr_file = strdup(filename);
            if (!redir->stderr_file) {
                ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
                return -1;
            }
            
            // Trim any excess whitespace
            str_trim(redir->stderr_file);
        }
    }
    
    return 0;
}

/**
 * @brief Parse command into argv array
 * 
 * Parses a command string into a Command structure with argc/argv.
 * 
 * @param input Input string to parse
 * @param cmd Pointer to Command structure to fill
 * @return int 0 on success, non-zero on error
 */
int parse_command(char *input, Command *cmd) {
    if (!input || !cmd) return -1;
    
    // Estimate max number of arguments (will be adjusted as needed)
    int max_args = 64;
    cmd->argv = malloc(max_args * sizeof(char*));
    if (!cmd->argv) {
        ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
        return -1;
    }
    
    // Initialize argv array
    memset(cmd->argv, 0, max_args * sizeof(char*));
    cmd->argc = 0;
    
    // Parse input with quote and backslash handling
    char *current = input;
    char arg[1024]; // Temporary buffer for building arguments
    int arg_pos = 0;
    int in_single_quotes = 0;
    int in_double_quotes = 0;
    
    // Trim leading whitespace
    while (isspace(*current)) current++;
    
    while (*current != '\0' && cmd->argc < (max_args - 1)) {
        // Handle backslash escaping - only outside single quotes
        if (*current == '\\' && !in_single_quotes) {
            if (in_double_quotes) {
                // In double quotes, backslash only escapes certain characters
                current++;
                if (*current == '\\' || *current == '"' || *current == '$' || *current == '\n') {
                    if (arg_pos < 1023) arg[arg_pos++] = *current;
                } else {
                    // Otherwise keep both the backslash and the character
                    if (arg_pos < 1023) arg[arg_pos++] = '\\';
                    if (*current != '\0' && arg_pos < 1023) {
                        arg[arg_pos++] = *current;
                    }
                }
            } else {
                // Outside quotes, backslash escapes any character
                current++;
                if (*current != '\0' && arg_pos < 1023) {
                    arg[arg_pos++] = *current;
                }
            }
        } else if (*current == '\'' && !in_double_quotes) {
            // Toggle single quote state
            in_single_quotes = !in_single_quotes;
        } else if (*current == '"' && !in_single_quotes) {
            // Toggle double quote state
            in_double_quotes = !in_double_quotes;
        } else if (isspace(*current) && !in_single_quotes && !in_double_quotes) {
            // Space outside quotes - end of argument
            if (arg_pos > 0) {
                arg[arg_pos] = '\0';
                cmd->argv[cmd->argc] = strdup(arg);
                if (!cmd->argv[cmd->argc]) {
                    ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
                    return -1;
                }
                cmd->argc++;
                arg_pos = 0;
            }
            
            // Skip consecutive spaces
            while (isspace(*current)) current++;
            continue;
        } else {
            // Regular character - add to current argument
            if (arg_pos < 1023) arg[arg_pos++] = *current;
        }
        
        current++;
    }
    
    // Check for unclosed quotes
    if (in_single_quotes || in_double_quotes) {
        ERROR_ERROR(ERR_SYNTAX, "Unclosed quotes in command");
        return -1;
    }
    
    // Handle the last argument if there is one
    if (arg_pos > 0) {
        arg[arg_pos] = '\0';
        cmd->argv[cmd->argc] = strdup(arg);
        if (!cmd->argv[cmd->argc]) {
            ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
            return -1;
        }
        cmd->argc++;
    }
    
    // NULL terminate the argument list
    cmd->argv[cmd->argc] = NULL;
    
    return 0;
}

/**
 * @brief Parse echo command arguments
 * 
 * Special handling for echo command arguments with escape sequences.
 * 
 * @param input Input arguments string
 * @param output Output buffer for processed arguments
 * @param output_size Size of output buffer
 */
void parse_echo_args(const char *input, char *output, size_t output_size) {
    if (!input || !output || output_size == 0) return;
    
    size_t in_pos = 0;
    size_t out_pos = 0;
    
    while (input[in_pos] != '\0' && out_pos < output_size - 1) {
        // Handle escape sequences
        if (input[in_pos] == '\\') {
            in_pos++;
            if (input[in_pos] == '\0') {
                // Backslash at end of string
                if (out_pos < output_size - 1) {
                    output[out_pos++] = '\\';
                }
                break;
            }
            
            // Process escape sequence
            switch (input[in_pos]) {
                case 'n':  // Newline
                    output[out_pos++] = '\n';
                    break;
                case 't':  // Tab
                    output[out_pos++] = '\t';
                    break;
                case 'r':  // Carriage return
                    output[out_pos++] = '\r';
                    break;
                case '\\': // Backslash
                    output[out_pos++] = '\\';
                    break;
                case '\'': // Single quote
                    output[out_pos++] = '\'';
                    break;
                case '"':  // Double quote
                    output[out_pos++] = '"';
                    break;
                default:   // Other characters
                    // Keep both the backslash and the character
                    if (out_pos < output_size - 2) {
                        output[out_pos++] = '\\';
                        output[out_pos++] = input[in_pos];
                    }
                    break;
            }
        } else {
            // Copy regular character
            output[out_pos++] = input[in_pos];
        }
        
        in_pos++;
    }
    
    // Null-terminate the output
    output[out_pos] = '\0';
}

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
size_t process_escapes(const char *input, char *output, size_t output_size) {
    if (!input || !output || output_size == 0) return 0;
    
    size_t in_pos = 0;
    size_t out_pos = 0;
    
    while (input[in_pos] != '\0' && out_pos < output_size - 1) {
        if (input[in_pos] == '\\') {
            in_pos++;
            if (input[in_pos] == '\0') {
                // Backslash at end of string
                break;
            }
            
            // Process escape sequence
            switch (input[in_pos]) {
                case 'n':  output[out_pos++] = '\n'; break;
                case 't':  output[out_pos++] = '\t'; break;
                case 'r':  output[out_pos++] = '\r'; break;
                case 'a':  output[out_pos++] = '\a'; break;
                case 'b':  output[out_pos++] = '\b'; break;
                case 'f':  output[out_pos++] = '\f'; break;
                case 'v':  output[out_pos++] = '\v'; break;
                case '\\': output[out_pos++] = '\\'; break;
                case '\'': output[out_pos++] = '\''; break;
                case '"':  output[out_pos++] = '"';  break;
                default:   output[out_pos++] = input[in_pos]; break;
            }
        } else {
            output[out_pos++] = input[in_pos];
        }
        
        in_pos++;
    }
    
    output[out_pos] = '\0';
    return out_pos;
}

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
size_t process_quotes(const char *input, char *output, size_t output_size) {
    if (!input || !output || output_size == 0) return 0;
    
    size_t in_pos = 0;
    size_t out_pos = 0;
    int in_single_quotes = 0;
    int in_double_quotes = 0;
    
    while (input[in_pos] != '\0' && out_pos < output_size - 1) {
        if (input[in_pos] == '\'' && !in_double_quotes) {
            // Toggle single quote state
            in_single_quotes = !in_single_quotes;
        } else if (input[in_pos] == '"' && !in_single_quotes) {
            // Toggle double quote state
            in_double_quotes = !in_double_quotes;
        } else {
            // Process character based on quote state
            if (input[in_pos] == '\\' && !in_single_quotes) {
                // Handle escape sequence
                in_pos++;
                if (input[in_pos] == '\0') break;
                
                if (in_double_quotes) {
                    // In double quotes, only certain characters are escaped
                    if (input[in_pos] == '\\' || input[in_pos] == '"' || 
                        input[in_pos] == '$' || input[in_pos] == '\n') {
                        output[out_pos++] = input[in_pos];
                    } else {
                        // Otherwise keep both the backslash and the character
                        if (out_pos < output_size - 2) {
                            output[out_pos++] = '\\';
                            output[out_pos++] = input[in_pos];
                        }
                    }
                } else {
                    // Outside quotes, all characters can be escaped
                    output[out_pos++] = input[in_pos];
                }
            } else {
                // Regular character
                output[out_pos++] = input[in_pos];
            }
        }
        
        in_pos++;
    }
    
    // Check for unclosed quotes
    if (in_single_quotes || in_double_quotes) {
        ERROR_ERROR(ERR_SYNTAX, "Unclosed quotes in string");
        return 0;
    }
    
    output[out_pos] = '\0';
    return out_pos;
}

/**
 * @brief Validates command syntax
 * 
 * Checks for syntax errors in a command string.
 * 
 * @param input Command string to validate
 * @return int 0 if syntax is valid, error code otherwise
 */
int validate_command_syntax(const char *input) {
    if (!input) return ERR_INVALID_ARG;
    
    int in_single_quotes = 0;
    int in_double_quotes = 0;
    const char *current = input;
    
    while (*current != '\0') {
        if (*current == '\\') {
            // Skip escaped character
            current++;
            if (*current == '\0') {
                // Backslash at end of string
                return ERR_SYNTAX;
            }
        } else if (*current == '\'' && !in_double_quotes) {
            in_single_quotes = !in_single_quotes;
        } else if (*current == '"' && !in_single_quotes) {
            in_double_quotes = !in_double_quotes;
        } else if (*current == '>' && !in_single_quotes && !in_double_quotes) {
            // Check for redirection syntax
            if (current == input) {
                // Redirection at start of command
                return ERR_SYNTAX;
            }
            
            // Check for '>>' syntax
            if (*(current + 1) == '>') {
                current++;
            }
            
            // Skip spaces after redirection
            current++;
            while (*current == ' ') current++;
            
            // Check for missing filename
            if (*current == '\0' || *current == '>') {
                return ERR_SYNTAX;
            }
        }
        
        current++;
    }
    
    // Check for unclosed quotes
    if (in_single_quotes || in_double_quotes) {
        return ERR_SYNTAX;
    }
    
    return 0;
}