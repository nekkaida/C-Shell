/**
 * @file matcher.c
 * @brief String matching implementation for tab completion
 *
 * Implementation of string matching functions for tab completion.
 */

#include "completion/matcher.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

/**
 * @brief Match prefix
 * 
 * Checks if a string starts with a given prefix.
 * 
 * @param string String to check
 * @param prefix Prefix to match
 * @return int 1 if match, 0 otherwise
 */
int prefix_match(const char *string, const char *prefix) {
    if (!string || !prefix) {
        return 0;
    }
    
    size_t prefix_len = strlen(prefix);
    size_t string_len = strlen(string);
    
    if (prefix_len > string_len) {
        return 0;
    }
    
    return strncmp(string, prefix, prefix_len) == 0;
}

/**
 * @brief Case-insensitive prefix match
 * 
 * Checks if a string starts with a given prefix, ignoring case.
 * 
 * @param string String to check
 * @param prefix Prefix to match
 * @return int 1 if match, 0 otherwise
 */
int prefix_match_case_insensitive(const char *string, const char *prefix) {
    if (!string || !prefix) {
        return 0;
    }
    
    size_t prefix_len = strlen(prefix);
    size_t string_len = strlen(string);
    
    if (prefix_len > string_len) {
        return 0;
    }
    
    for (size_t i = 0; i < prefix_len; i++) {
        if (tolower((unsigned char)string[i]) != tolower((unsigned char)prefix[i])) {
            return 0;
        }
    }
    
    return 1;
}

/**
 * @brief Compare strings for sorting
 * 
 * Comparison function for qsort.
 * 
 * @param a First string
 * @param b Second string
 * @return int Comparison result
 */
int compare_strings(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

/**
 * @brief Find common prefix length
 * 
 * Finds the length of the common prefix between two strings.
 * 
 * @param s1 First string
 * @param s2 Second string
 * @return int Length of common prefix
 */
int common_prefix_length(const char *s1, const char *s2) {
    if (!s1 || !s2) {
        return 0;
    }
    
    int i = 0;
    while (s1[i] && s2[i] && s1[i] == s2[i]) {
        i++;
    }
    
    return i;
}

/**
 * @brief Check if string is a directory
 * 
 * Checks if a path refers to a directory.
 * 
 * @param path Path to check
 * @return int 1 if directory, 0 otherwise
 */
int is_directory(const char *path) {
    if (!path) {
        return 0;
    }
    
    struct stat st;
    
    if (stat(path, &st) != 0) {
        return 0;
    }
    
    return S_ISDIR(st.st_mode);
}

/**
 * @brief Format directory completion
 * 
 * Formats a directory name for completion (adds trailing slash).
 * 
 * @param path Directory path
 * @return char* Formatted path (dynamically allocated, caller must free)
 */
char *format_directory_completion(const char *path) {
    if (!path) {
        return NULL;
    }
    
    // Check if it's a directory
    if (!is_directory(path)) {
        return strdup(path);
    }
    
    // Check if it already has a trailing slash
    size_t len = strlen(path);
    if (len > 0 && path[len - 1] == '/') {
        return strdup(path);
    }
    
    // Add trailing slash
    char *result = malloc(len + 2);
    if (!result) {
        return NULL;
    }
    
    strcpy(result, path);
    result[len] = '/';
    result[len + 1] = '\0';
    
    return result;
}

/**
 * @brief Filter completions
 * 
 * Filters a list of completions based on a predicate function.
 * 
 * @param completions Array of completions
 * @param num_completions Pointer to number of completions
 * @param predicate Function to check if a completion should be kept
 * @return int New number of completions
 */
int filter_completions(char **completions, int *num_completions, int (*predicate)(const char *)) {
    if (!completions || !num_completions || !predicate) {
        return -1;
    }
    
    int new_count = 0;
    
    for (int i = 0; i < *num_completions; i++) {
        if (predicate(completions[i])) {
            // Keep this completion
            if (new_count != i) {
                // Move it to the new position
                completions[new_count] = completions[i];
            }
            new_count++;
        } else {
            // Free this completion
            free(completions[i]);
        }
    }
    
    *num_completions = new_count;
    return new_count;
}

/**
 * @brief Sort and deduplicate completions
 * 
 * Sorts completions alphabetically and removes duplicates.
 * 
 * @param completions Array of completions
 * @param num_completions Pointer to number of completions
 * @return int New number of completions
 */
int sort_and_deduplicate(char **completions, int *num_completions) {
    if (!completions || !num_completions || *num_completions <= 0) {
        return -1;
    }
    
    // Sort completions
    qsort(completions, *num_completions, sizeof(char *), compare_strings);
    
    // Deduplicate
    int new_count = 1;
    
    for (int i = 1; i < *num_completions; i++) {
        if (strcmp(completions[i], completions[new_count - 1]) != 0) {
            // Not a duplicate
            if (new_count != i) {
                // Move it to the new position
                completions[new_count] = completions[i];
            }
            new_count++;
        } else {
            // Duplicate - free it
            free(completions[i]);
        }
    }
    
    *num_completions = new_count;
    return new_count;
}