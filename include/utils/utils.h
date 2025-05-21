/**
 * @file utils.h
 * @brief Utility functions
 *
 * Utility functions used throughout the shell.
 */

#ifndef UTILS_H
#define UTILS_H

/**
 * @brief Safe string copy
 * 
 * Copies a string with boundary checks.
 * 
 * @param dest Destination buffer
 * @param src Source string
 * @param size Buffer size
 * @return char* Destination buffer
 */
char *safe_strcpy(char *dest, const char *src, size_t size);

/**
 * @brief Safe string concatenation
 * 
 * Concatenates strings with boundary checks.
 * 
 * @param dest Destination buffer
 * @param src Source string
 * @param size Buffer size
 * @return char* Destination buffer
 */
char *safe_strcat(char *dest, const char *src, size_t size);

/**
 * @brief Duplicate a string
 * 
 * Duplicates a string (caller must free).
 * 
 * @param str String to duplicate
 * @return char* Duplicated string
 */
char *safe_strdup(const char *str);

/**
 * @brief Create a concatenated string
 * 
 * Creates a new string by concatenating two strings.
 * 
 * @param s1 First string
 * @param s2 Second string
 * @return char* New string (caller must free)
 */
char *str_join(const char *s1, const char *s2);

/**
 * @brief Trim whitespace
 * 
 * Removes leading and trailing whitespace.
 * 
 * @param str String to trim
 * @return char* Trimmed string (statically allocated)
 */
char *str_trim(char *str);

/**
 * @brief Split string
 * 
 * Splits a string by delimiter.
 * 
 * @param str String to split
 * @param delim Delimiter
 * @param count Pointer to store number of parts
 * @return char** Array of strings (caller must free)
 */
char **str_split(const char *str, char delim, int *count);

/**
 * @brief Free split result
 * 
 * Frees memory allocated by str_split.
 * 
 * @param parts Array of strings
 * @param count Number of parts
 */
void free_str_array(char **parts, int count);

/**
 * @brief Check if string starts with prefix
 * 
 * @param str String to check
 * @param prefix Prefix to match
 * @return int 1 if match, 0 otherwise
 */
int str_starts_with(const char *str, const char *prefix);

/**
 * @brief Check if string ends with suffix
 * 
 * @param str String to check
 * @param suffix Suffix to match
 * @return int 1 if match, 0 otherwise
 */
int str_ends_with(const char *str, const char *suffix);

/**
 * @brief Replace substring
 * 
 * Replaces all occurrences of a substring.
 * 
 * @param str String to modify
 * @param old_substr Substring to replace
 * @param new_substr Replacement string
 * @return char* New string (caller must free)
 */
char *str_replace(const char *str, const char *old_substr, const char *new_substr);

/**
 * @brief Expand tilde in path
 * 
 * Expands ~ to home directory in a path.
 * 
 * @param path Path to expand
 * @return char* Expanded path (caller must free)
 */
char *expand_tilde(const char *path);

/**
 * @brief Get absolute path
 * 
 * Converts a path to absolute path.
 * 
 * @param path Path to convert
 * @return char* Absolute path (caller must free)
 */
char *get_absolute_path(const char *path);

/**
 * @brief Create directory path
 * 
 * Creates a directory path (including parent directories).
 * 
 * @param path Path to create
 * @param mode Directory permissions
 * @return int 0 on success, -1 on error
 */
int create_path(const char *path, mode_t mode);

/**
 * @brief Check if file exists
 * 
 * @param path File path
 * @return int 1 if exists, 0 otherwise
 */
int file_exists(const char *path);

/**
 * @brief Print error message
 * 
 * Prints an error message to stderr.
 * 
 * @param format Format string
 * @param ... Additional arguments
 */
void print_error(const char *format, ...);

/**
 * @brief Print debug message
 * 
 * Prints a debug message if verbose mode is enabled.
 * 
 * @param format Format string
 * @param ... Additional arguments
 */
void print_debug(const char *format, ...);

#endif /* UTILS_H */