/**
 * @file string.h
 * @brief String utilities
 *
 * Functions for string manipulation.
 */

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stddef.h>

/**
 * @brief Duplicate a string
 * 
 * Creates a copy of a string.
 * 
 * @param str String to duplicate
 * @return char* Duplicated string (caller must free)
 */
char *str_dup(const char *str);

/**
 * @brief Duplicate up to n characters of a string
 * 
 * Creates a copy of up to n characters of a string.
 * 
 * @param str String to duplicate
 * @param n Maximum number of characters
 * @return char* Duplicated string (caller must free)
 */
char *str_ndup(const char *str, size_t n);

/**
 * @brief Trim left whitespace
 * 
 * Removes leading whitespace from a string.
 * 
 * @param str String to trim
 * @return char* Pointer to first non-whitespace character
 */
char *str_trim_left(char *str);

/**
 * @brief Trim right whitespace
 * 
 * Removes trailing whitespace from a string.
 * 
 * @param str String to trim
 * @return char* Original string pointer
 */
char *str_trim_right(char *str);

/**
 * @brief Trim whitespace
 * 
 * Removes leading and trailing whitespace.
 * 
 * @param str String to trim
 * @return char* Trimmed string (same as input)
 */
char *str_trim(char *str);

/**
 * @brief Create a new trimmed string
 * 
 * Creates a new string with leading and trailing whitespace removed.
 * 
 * @param str String to trim
 * @return char* New trimmed string (caller must free)
 */
char *str_trim_dup(const char *str);

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
void str_split_free(char **parts, int count);

/**
 * @brief Join strings
 * 
 * Joins an array of strings with a delimiter.
 * 
 * @param parts Array of strings
 * @param count Number of parts
 * @param delim Delimiter
 * @return char* Joined string (caller must free)
 */
char *str_join(char **parts, int count, const char *delim);

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
 * @brief Find first occurrence of character
 * 
 * Finds the first occurrence of a character in a string.
 * 
 * @param str String to search
 * @param c Character to find
 * @return char* Pointer to first occurrence, NULL if not found
 */
char *str_chr(const char *str, int c);

/**
 * @brief Find last occurrence of character
 * 
 * Finds the last occurrence of a character in a string.
 * 
 * @param str String to search
 * @param c Character to find
 * @return char* Pointer to last occurrence, NULL if not found
 */
char *str_rchr(const char *str, int c);

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
 * @brief Copy string with size limit
 * 
 * Copies a string with a size limit, ensuring null termination.
 * 
 * @param dst Destination buffer
 * @param src Source string
 * @param size Buffer size
 * @return size_t Number of characters copied
 */
size_t str_copy(char *dst, const char *src, size_t size);

/**
 * @brief Concatenate string with size limit
 * 
 * Concatenates a string with a size limit, ensuring null termination.
 * 
 * @param dst Destination buffer
 * @param src Source string
 * @param size Buffer size
 * @return size_t Number of characters in result
 */
size_t str_concat(char *dst, const char *src, size_t size);

/**
 * @brief Compare strings case-insensitive
 * 
 * Compares two strings, ignoring case.
 * 
 * @param s1 First string
 * @param s2 Second string
 * @return int 0 if equal, negative if s1 < s2, positive if s1 > s2
 */
int str_casecmp(const char *s1, const char *s2);

/**
 * @brief Compare strings case-insensitive up to n characters
 * 
 * @param s1 First string
 * @param s2 Second string
 * @param n Maximum number of characters to compare
 * @return int 0 if equal, negative if s1 < s2, positive if s1 > s2
 */
int str_ncasecmp(const char *s1, const char *s2, size_t n);

#endif /* STRING_UTILS_H */