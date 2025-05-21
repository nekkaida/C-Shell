/**
 * @file completion.h
 * @brief Tab completion component
 *
 * Functions for handling tab completion in the shell.
 */

#ifndef COMPLETION_H
#define COMPLETION_H

/**
 * @brief Handle tab completion
 * 
 * Main function to handle tab completion for the current input.
 * 
 * @param input Input buffer
 * @param cursor_pos Pointer to cursor position
 */
void handle_tab_completion(char *input, int *cursor_pos);

/**
 * @brief Get all possible completions
 * 
 * Find all possible completions for a given prefix.
 * 
 * @param prefix Prefix to complete
 * @param completions_ptr Pointer to store completions array
 * @param num_completions_ptr Pointer to store number of completions
 */
void get_all_completions(const char *prefix, char ***completions_ptr, int *num_completions_ptr);

/**
 * @brief Find the longest common prefix
 * 
 * Finds the longest common prefix among a set of strings.
 * 
 * @param strings Array of strings
 * @param count Number of strings
 * @return char* Longest common prefix (dynamically allocated, caller must free)
 */
char* find_longest_common_prefix(char **strings, int count);

/**
 * @brief Display all possible completions
 * 
 * Displays all possible completions when there are multiple matches.
 * 
 * @param completions Array of completions
 * @param num_completions Number of completions
 * @param input Current input buffer
 */
void display_completions(char **completions, int num_completions, const char *input);

/**
 * @brief Free completions array
 * 
 * Frees memory allocated for completions.
 * 
 * @param completions Array of completions
 * @param num_completions Number of completions
 */
void free_completions(char **completions, int num_completions);

/**
 * @brief Get directory entries matching a prefix
 * 
 * Gets directory entries that match a given prefix.
 * 
 * @param dir_path Directory path
 * @param prefix Prefix to match
 * @param completions_ptr Pointer to store completions
 * @param num_completions_ptr Pointer to store number of completions
 * @return int 0 on success, non-zero on error
 */
int get_directory_completions(const char *dir_path, const char *prefix, 
                             char ***completions_ptr, int *num_completions_ptr);

/**
 * @brief Get command completions
 * 
 * Gets command completions from PATH and built-ins.
 * 
 * @param prefix Prefix to match
 * @param completions_ptr Pointer to store completions
 * @param num_completions_ptr Pointer to store number of completions
 * @return int 0 on success, non-zero on error
 */
int get_command_completions(const char *prefix, char ***completions_ptr, int *num_completions_ptr);

/**
 * @brief Extract last word from input
 * 
 * Extracts the last word from the input string for completion.
 * 
 * @param input Input string
 * @return char* Last word (dynamically allocated, caller must free)
 */
char *extract_last_word(const char *input);

/**
 * @brief Add a completion to a list
 * 
 * Adds a completion to a list, checking for duplicates.
 * 
 * @param completions Array of completions (may be reallocated)
 * @param capacity Pointer to current capacity
 * @param count Pointer to current count
 * @param completion Completion to add
 * @return int 0 on success, non-zero on error
 */
int add_completion(char ***completions, int *capacity, int *count, const char *completion);

#endif /* COMPLETION_H */