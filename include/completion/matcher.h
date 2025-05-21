/**
 * @file matcher.h
 * @brief String matching for tab completion
 *
 * Functions for matching strings for tab completion.
 */

#ifndef MATCHER_H
#define MATCHER_H

/**
 * @brief Match prefix
 * 
 * Checks if a string starts with a given prefix.
 * 
 * @param string String to check
 * @param prefix Prefix to match
 * @return int 1 if match, 0 otherwise
 */
int prefix_match(const char *string, const char *prefix);

/**
 * @brief Case-insensitive prefix match
 * 
 * Checks if a string starts with a given prefix, ignoring case.
 * 
 * @param string String to check
 * @param prefix Prefix to match
 * @return int 1 if match, 0 otherwise
 */
int prefix_match_case_insensitive(const char *string, const char *prefix);

/**
 * @brief Compare strings for sorting
 * 
 * Comparison function for qsort.
 * 
 * @param a First string
 * @param b Second string
 * @return int Comparison result
 */
int compare_strings(const void *a, const void *b);

/**
 * @brief Find common prefix length
 * 
 * Finds the length of the common prefix between two strings.
 * 
 * @param s1 First string
 * @param s2 Second string
 * @return int Length of common prefix
 */
int common_prefix_length(const char *s1, const char *s2);

/**
 * @brief Check if string is a directory
 * 
 * Checks if a path refers to a directory.
 * 
 * @param path Path to check
 * @return int 1 if directory, 0 otherwise
 */
int is_directory(const char *path);

/**
 * @brief Format directory completion
 * 
 * Formats a directory name for completion (adds trailing slash).
 * 
 * @param path Directory path
 * @return char* Formatted path (dynamically allocated, caller must free)
 */
char *format_directory_completion(const char *path);

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
int filter_completions(char **completions, int *num_completions, int (*predicate)(const char *));

/**
 * @brief Sort and deduplicate completions
 * 
 * Sorts completions alphabetically and removes duplicates.
 * 
 * @param completions Array of completions
 * @param num_completions Pointer to number of completions
 * @return int New number of completions
 */
int sort_and_deduplicate(char **completions, int *num_completions);

#endif /* MATCHER_H */