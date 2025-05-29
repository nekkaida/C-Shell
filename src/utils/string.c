/**
 * @file string.c
 * @brief String utilities implementation
 *
 * Implementation of string manipulation functions.
 */

#include "utils/string.h"
#include "utils/error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Duplicate a string
 * 
 * Creates a copy of a string.
 * 
 * @param str String to duplicate
 * @return char* Duplicated string (caller must free)
 */
char *str_dup(const char *str) {
    if (!str) {
        return NULL;
    }
    
    char *result = strdup(str);
    if (!result) {
        ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
    }
    
    return result;
}

/**
 * @brief Duplicate up to n characters of a string
 * 
 * Creates a copy of up to n characters of a string.
 * 
 * @param str String to duplicate
 * @param n Maximum number of characters
 * @return char* Duplicated string (caller must free)
 */
char *str_ndup(const char *str, size_t n) {
    if (!str) {
        return NULL;
    }
    
    // Allocate memory
    size_t len = strlen(str);
    if (len > n) {
        len = n;
    }
    
    char *result = (char *)malloc(len + 1);
    if (!result) {
        ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
        return NULL;
    }
    
    // Copy the string
    memcpy(result, str, len);
    result[len] = '\0';
    
    return result;
}

/**
 * @brief Trim left whitespace
 * 
 * Removes leading whitespace from a string.
 * 
 * @param str String to trim
 * @return char* Pointer to first non-whitespace character
 */
char *str_trim_left(char *str) {
    if (!str) {
        return NULL;
    }
    
    // Skip leading whitespace
    while (*str && isspace((unsigned char)*str)) {
        str++;
    }
    
    return str;
}

/**
 * @brief Trim right whitespace
 * 
 * Removes trailing whitespace from a string.
 * 
 * @param str String to trim
 * @return char* Original string pointer
 */
char *str_trim_right(char *str) {
    if (!str) {
        return NULL;
    }
    
    // Find the end of the string
    char *end = str + strlen(str);
    
    // Backtrack to find the last non-whitespace character
    while (end > str && isspace((unsigned char)*(end - 1))) {
        end--;
    }
    
    // Null-terminate the string at the new position
    *end = '\0';
    
    return str;
}

/**
 * @brief Trim whitespace
 * 
 * Removes leading and trailing whitespace.
 * 
 * @param str String to trim
 * @return char* Trimmed string (same as input)
 */
char *str_trim(char *str) {
    if (!str) {
        return NULL;
    }
    
    // First trim right to remove trailing whitespace
    str_trim_right(str);
    
    // Then return the result of trimming left
    return str_trim_left(str);
}

/**
 * @brief Create a new trimmed string
 * 
 * Creates a new string with leading and trailing whitespace removed.
 * 
 * @param str String to trim
 * @return char* New trimmed string (caller must free)
 */
char *str_trim_dup(const char *str) {
    if (!str) {
        return NULL;
    }
    
    // Make a copy of the string
    char *copy = str_dup(str);
    if (!copy) {
        return NULL;
    }
    
    // Trim the copy
    str_trim(copy);
    
    return copy;
}

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
char **str_split(const char *str, char delim, int *count) {
    if (!str || !count) {
        return NULL;
    }
    
    // Initialize count
    *count = 0;
    
    // Count the number of delimiters
    const char *s = str;
    while (*s) {
        if (*s == delim) {
            (*count)++;
        }
        s++;
    }
    
    // Number of parts is number of delimiters + 1
    (*count)++;
    
    // Allocate array of strings
    char **result = (char **)malloc(*count * sizeof(char *));
    if (!result) {
        ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
        *count = 0;
        return NULL;
    }
    
    // Initialize array
    for (int i = 0; i < *count; i++) {
        result[i] = NULL;
    }
    
    // Split the string
    const char *start = str;
    int i = 0;
    
    s = str;
    while (*s) {
        if (*s == delim) {
            // Calculate the length of this part
            size_t len = s - start;
            
            // Allocate memory for this part
            result[i] = (char *)malloc(len + 1);
            if (!result[i]) {
                // Error allocating memory
                str_split_free(result, i);
                *count = 0;
                return NULL;
            }
            
            // Copy this part
            memcpy(result[i], start, len);
            result[i][len] = '\0';
            
            // Move to next part
            start = s + 1;
            i++;
        }
        s++;
    }
    
    // Handle the last part
    size_t len = s - start;
    result[i] = (char *)malloc(len + 1);
    if (!result[i]) {
        // Error allocating memory
        str_split_free(result, i);
        *count = 0;
        return NULL;
    }
    
    // Copy the last part
    memcpy(result[i], start, len);
    result[i][len] = '\0';
    
    return result;
}

/**
 * @brief Free split result
 * 
 * Frees memory allocated by str_split.
 * 
 * @param parts Array of strings
 * @param count Number of parts
 */
void str_split_free(char **parts, int count) {
    if (!parts) {
        return;
    }
    
    for (int i = 0; i < count; i++) {
        free(parts[i]);
    }
    
    free(parts);
}

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
char *str_join(char **parts, int count, const char *delim) {
    if (!parts || count <= 0 || !delim) {
        return NULL;
    }
    
    // Calculate the total length
    size_t total_len = 0;
    size_t delim_len = strlen(delim);
    
    for (int i = 0; i < count; i++) {
        if (parts[i]) {
            total_len += strlen(parts[i]);
        }
        
        // Add delimiter length (except for the last part)
        if (i < count - 1) {
            total_len += delim_len;
        }
    }
    
    // Allocate memory for the result
    char *result = (char *)malloc(total_len + 1);
    if (!result) {
        ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
        return NULL;
    }
    
    // Join the parts
    char *p = result;
    
    for (int i = 0; i < count; i++) {
        if (parts[i]) {
            size_t len = strlen(parts[i]);
            memcpy(p, parts[i], len);
            p += len;
        }
        
        // Add delimiter (except for the last part)
        if (i < count - 1) {
            memcpy(p, delim, delim_len);
            p += delim_len;
        }
    }
    
    // Null-terminate the result
    *p = '\0';
    
    return result;
}

/**
 * @brief Check if string starts with prefix
 * 
 * @param str String to check
 * @param prefix Prefix to match
 * @return int 1 if match, 0 otherwise
 */
int str_starts_with(const char *str, const char *prefix) {
    if (!str || !prefix) {
        return 0;
    }
    
    size_t prefix_len = strlen(prefix);
    size_t str_len = strlen(str);
    
    if (prefix_len > str_len) {
        return 0;
    }
    
    return strncmp(str, prefix, prefix_len) == 0;
}

/**
 * @brief Check if string ends with suffix
 * 
 * @param str String to check
 * @param suffix Suffix to match
 * @return int 1 if match, 0 otherwise
 */
int str_ends_with(const char *str, const char *suffix) {
    if (!str || !suffix) {
        return 0;
    }
    
    size_t suffix_len = strlen(suffix);
    size_t str_len = strlen(str);
    
    if (suffix_len > str_len) {
        return 0;
    }
    
    return strcmp(str + str_len - suffix_len, suffix) == 0;
}

/**
 * @brief Find first occurrence of character
 * 
 * Finds the first occurrence of a character in a string.
 * 
 * @param str String to search
 * @param c Character to find
 * @return char* Pointer to first occurrence, NULL if not found
 */
char *str_chr(const char *str, int c) {
    if (!str) {
        return NULL;
    }
    
    return strchr(str, c);
}

/**
 * @brief Find last occurrence of character
 * 
 * Finds the last occurrence of a character in a string.
 * 
 * @param str String to search
 * @param c Character to find
 * @return char* Pointer to last occurrence, NULL if not found
 */
char *str_rchr(const char *str, int c) {
    if (!str) {
        return NULL;
    }
    
    return strrchr(str, c);
}

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
char *str_replace(const char *str, const char *old_substr, const char *new_substr) {
    if (!str || !old_substr || !new_substr) {
        return NULL;
    }
    
    // Get the lengths
    size_t str_len = strlen(str);
    size_t old_len = strlen(old_substr);
    size_t new_len = strlen(new_substr);
    
    // Special case: empty old substring
    if (old_len == 0) {
        return str_dup(str);
    }
    
    // Count the number of occurrences
    size_t count = 0;
    const char *p = str;
    
    while ((p = strstr(p, old_substr)) != NULL) {
        count++;
        p += old_len;
    }
    
    // No occurrences, just duplicate the string
    if (count == 0) {
        return str_dup(str);
    }
    
    // Calculate the new string length
    size_t result_len = str_len + count * (new_len - old_len);
    
    // Allocate memory for the result
    char *result = (char *)malloc(result_len + 1);
    if (!result) {
        ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
        return NULL;
    }
    
    // Replace all occurrences
    char *dst = result;
    p = str;
    
    while (1) {
        const char *occur = strstr(p, old_substr);
        
        if (occur) {
            // Copy up to the occurrence
            size_t chunk_len = occur - p;
            memcpy(dst, p, chunk_len);
            dst += chunk_len;
            
            // Copy the replacement
            memcpy(dst, new_substr, new_len);
            dst += new_len;
            
            p = occur + old_len;
        } else {
            // Copy the rest of the string
            strcpy(dst, p);
            break;
        }
    }
    
    return result;
}

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
size_t str_copy(char *dst, const char *src, size_t size) {
    if (!dst || !src || size == 0) {
        return 0;
    }
    
    size_t i;
    for (i = 0; i < size - 1 && src[i]; i++) {
        dst[i] = src[i];
    }
    
    dst[i] = '\0';
    return i;
}

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
size_t str_concat(char *dst, const char *src, size_t size) {
    if (!dst || !src || size == 0) {
        return 0;
    }
    
    size_t dst_len = strlen(dst);
    if (dst_len >= size - 1) {
        return dst_len;
    }
    
    size_t i;
    for (i = 0; i < size - dst_len - 1 && src[i]; i++) {
        dst[dst_len + i] = src[i];
    }
    
    dst[dst_len + i] = '\0';
    return dst_len + i;
}

/**
 * @brief Compare strings case-insensitive
 * 
 * Compares two strings, ignoring case.
 * 
 * @param s1 First string
 * @param s2 Second string
 * @return int 0 if equal, negative if s1 < s2, positive if s1 > s2
 */
int str_casecmp(const char *s1, const char *s2) {
    if (!s1 && !s2) {
        return 0;
    }
    if (!s1) {
        return -1;
    }
    if (!s2) {
        return 1;
    }
    
    unsigned char c1, c2;
    
    do {
        c1 = (unsigned char) *s1++;
        c2 = (unsigned char) *s2++;
        
        if (c1 == '\0') {
            break;
        }
        
        if (c1 == c2) {
            continue;
        }
        
        c1 = tolower(c1);
        c2 = tolower(c2);
        
        if (c1 != c2) {
            break;
        }
    } while (1);
    
    return c1 - c2;
}

/**
 * @brief Compare strings case-insensitive up to n characters
 * 
 * @param s1 First string
 * @param s2 Second string
 * @param n Maximum number of characters to compare
 * @return int 0 if equal, negative if s1 < s2, positive if s1 > s2
 */
int str_ncasecmp(const char *s1, const char *s2, size_t n) {
    if (n == 0) {
        return 0;
    }
    
    if (!s1 && !s2) {
        return 0;
    }
    if (!s1) {
        return -1;
    }
    if (!s2) {
        return 1;
    }
    
    unsigned char c1, c2;
    
    do {
        c1 = (unsigned char) *s1++;
        c2 = (unsigned char) *s2++;
        
        if (c1 == '\0' || --n == 0) {
            break;
        }
        
        if (c1 == c2) {
            continue;
        }
        
        c1 = tolower(c1);
        c2 = tolower(c2);
        
        if (c1 != c2) {
            break;
        }
    } while (1);
    
    return c1 - c2;
}