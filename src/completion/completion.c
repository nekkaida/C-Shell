/**
 * @file completion.c
 * @brief Tab completion implementation
 *
 * Implementation of tab completion functions.
 */

#include "completion/completion.h"
#include "completion/matcher.h"
#include "builtins/builtins.h"
#include "utils/error.h"
#include "utils/path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <time.h>
#include <sys/stat.h>

// Track tab state for double-tab behavior
static time_t last_tab_time = 0;
static char last_tab_prefix[PATH_MAX] = "";

/**
 * @brief Handle tab completion
 * 
 * Main function to handle tab completion for the current input.
 * 
 * @param input Input buffer
 * @param cursor_pos Pointer to cursor position
 */
void handle_tab_completion(char *input, int *cursor_pos) {
    // Current time for double-tab detection
    time_t current_time = time(NULL);
    
    // Only try to complete if we have text to work with
    if (*cursor_pos > 0) {
        // Create a copy of the input up to the cursor position
        char prefix[PATH_MAX];
        strncpy(prefix, input, *cursor_pos);
        prefix[*cursor_pos] = '\0';
        
        // Find the start of the last word (command)
        char *last_word = prefix;
        char *space = strrchr(prefix, ' ');
        if (space) {
            last_word = space + 1;
        }
        
        // Get list of possible completions
        char **completions = NULL;
        int num_completions = 0;
        
        // Get completions
        get_all_completions(last_word, &completions, &num_completions);
        
        if (num_completions == 0) {
            printf("\a");  // Ring the bell
            fflush(stdout);
        } 
        else if (num_completions == 1) {
            // Create the new input by replacing the last word with the completion
            char new_input[PATH_MAX] = {0};
            
            // Copy everything before the last word
            int prefix_length = last_word - prefix;
            if (prefix_length > 0) {
                strncpy(new_input, prefix, prefix_length);
            }
            
            // Append the completion
            strcat(new_input, completions[0]);
            
            // Add a space after the command if it's not a directory
            if (!is_directory(completions[0])) {
                strcat(new_input, " ");
            }
            
            // Calculate the new cursor position
            int new_cursor_pos = strlen(new_input);
            
            // Update the input and cursor
            strcpy(input, new_input);
            *cursor_pos = new_cursor_pos;
            
            // Visually update the command line
            // First, go back to the start of the line
            printf("\r$ ");
            
            // Then print the new input
            printf("%s", input);
            
            // Clear any remaining characters from the previous input
            int old_len = strlen(input);
            int clear_len = strlen(prefix) - old_len;
            for (int i = 0; i < clear_len + 1; i++) {
                printf(" ");
            }
            
            // Go back to where the cursor should be
            printf("\r$ %s", input);
            fflush(stdout);
            
            // Reset tab state
            last_tab_time = 0;
            last_tab_prefix[0] = '\0';
        } 
        else {
            if (current_time - last_tab_time <= 1 && 
                strcmp(last_tab_prefix, last_word) == 0) {
                // Second tab press within 1 second - show all completions
                printf("\n");
                
                // Print all completions with two spaces between each
                for (int i = 0; i < num_completions; i++) {
                    printf("%s", completions[i]);
                    if (i < num_completions - 1) {
                        printf("  ");  // Two spaces between completions
                    }
                }
                
                // Print a new prompt
                printf("\n$ %s", input);
                fflush(stdout);
                
                // Reset tab state
                last_tab_time = 0;
                last_tab_prefix[0] = '\0';
            } else {
                // Find the longest common prefix among the completions
                char *lcp = find_longest_common_prefix(completions, num_completions);
                
                if (lcp && strlen(lcp) > strlen(last_word)) {
                    // Create the new input by replacing the last word with the LCP
                    char new_input[PATH_MAX] = {0};
                    
                    // Copy everything before the last word
                    int prefix_length = last_word - prefix;
                    if (prefix_length > 0) {
                        strncpy(new_input, prefix, prefix_length);
                    }
                    
                    // Append the longest common prefix
                    strcat(new_input, lcp);
                    
                    int new_cursor_pos = strlen(new_input);
                    
                    // Update the input and cursor
                    strcpy(input, new_input);
                    *cursor_pos = new_cursor_pos;
                    
                    // Visually update the command line
                    printf("\r$ ");
                    printf("%s", input);
                    
                    // Clear any remaining characters from the previous input
                    int old_len = strlen(input);
                    int clear_len = strlen(prefix) - old_len;
                    for (int i = 0; i < clear_len + 1; i++) {
                        printf(" ");
                    }
                    
                    // Go back to where the cursor should be
                    printf("\r$ %s", input);
                    fflush(stdout);
                } else {
                    // No common prefix longer than the current text - ring bell
                    printf("\a");
                    fflush(stdout);
                }
                
                // Free the longest common prefix
                if (lcp) free(lcp);
                
                // Store tab state for next time
                last_tab_time = current_time;
                strcpy(last_tab_prefix, last_word);
            }
        }
        
        // Free completions
        if (completions) {
            for (int i = 0; i < num_completions; i++) {
                free(completions[i]);
            }
            free(completions);
        }
    }
}

/**
 * @brief Get all possible completions
 * 
 * Find all possible completions for a given prefix.
 * 
 * @param prefix Prefix to complete
 * @param completions_ptr Pointer to store completions array
 * @param num_completions_ptr Pointer to store number of completions
 */
void get_all_completions(const char *prefix, char ***completions_ptr, int *num_completions_ptr) {
    if (!prefix || !completions_ptr || !num_completions_ptr) return;
    
    // Initialize output parameters
    *completions_ptr = NULL;
    *num_completions_ptr = 0;
    
    // Array to store completions
    char **completions = NULL;
    int capacity = 10;  // Initial capacity
    int count = 0;
    
    // Allocate initial array
    completions = malloc(capacity * sizeof(char*));
    if (!completions) {
        ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
        return;
    }
    
    // Check if the prefix contains a slash - if so, it's a path
    if (strchr(prefix, '/') != NULL) {
        // Path completion
        char *last_slash = strrchr(prefix, '/');
        char dir_path[PATH_MAX];
        char file_prefix[PATH_MAX];
        
        // Extract directory path and file prefix
        if (last_slash == prefix) {
            // Root directory
            strcpy(dir_path, "/");
            strcpy(file_prefix, last_slash + 1);
        } else {
            // Other directory
            strncpy(dir_path, prefix, last_slash - prefix);
            dir_path[last_slash - prefix] = '\0';
            strcpy(file_prefix, last_slash + 1);
        }
        
        // Get completions from directory
        get_directory_completions(dir_path, file_prefix, &completions, &count);
    } else {
        // Check if we're completing the first word (command) or an argument
        if (prefix == extract_last_word(prefix)) {
            // First word - command completion
            // Check built-in commands
            const BuiltinCommand *builtins = get_builtin_commands();
            for (int i = 0; builtins[i].name != NULL; i++) {
                if (prefix_match(builtins[i].name, prefix)) {
                    // Add to completions array (resize if needed)
                    if (count >= capacity) {
                        capacity *= 2;
                        char **new_completions = realloc(completions, capacity * sizeof(char*));
                        if (!new_completions) {
                            // Free existing completions on error
                            for (int j = 0; j < count; j++) {
                                free(completions[j]);
                            }
                            free(completions);
                            *completions_ptr = NULL;
                            *num_completions_ptr = 0;
                            return;
                        }
                        completions = new_completions;
                    }
                    
                    completions[count] = strdup(builtins[i].name);
                    if (completions[count]) {
                        count++;
                    }
                }
            }
            
            // Check PATH for executables
            char *path_env = getenv("PATH");
            if (path_env) {
                char *path_copy = strdup(path_env);
                if (path_copy) {
                    char *path = strtok(path_copy, ":");
                    
                    while (path != NULL) {
                        DIR *dir = opendir(path);
                        if (dir) {
                            struct dirent *entry;
                            while ((entry = readdir(dir)) != NULL) {
                                if (prefix_match(entry->d_name, prefix)) {
                                    char full_path[PATH_MAX];
                                    snprintf(full_path, PATH_MAX, "%s/%s", path, entry->d_name);
                                    
                                    struct stat st;
                                    if (stat(full_path, &st) == 0 && 
                                        S_ISREG(st.st_mode) && 
                                        (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
                                        
                                        // Check for duplicates
                                        int duplicate = 0;
                                        for (int i = 0; i < count; i++) {
                                            if (strcmp(entry->d_name, completions[i]) == 0) {
                                                duplicate = 1;
                                                break;
                                            }
                                        }
                                        
                                        if (!duplicate) {
                                            // Add to completions
                                            if (count >= capacity) {
                                                capacity *= 2;
                                                char **new_completions = realloc(completions, capacity * sizeof(char*));
                                                if (!new_completions) {
                                                    for (int j = 0; j < count; j++) free(completions[j]);
                                                    free(completions);
                                                    *completions_ptr = NULL;
                                                    *num_completions_ptr = 0;
                                                    closedir(dir);
                                                    free(path_copy);
                                                    return;
                                                }
                                                completions = new_completions;
                                            }
                                            
                                            completions[count] = strdup(entry->d_name);
                                            if (completions[count]) count++;
                                        }
                                    }
                                }
                            }
                            closedir(dir);
                        }
                        path = strtok(NULL, ":");
                    }
                    free(path_copy);
                }
            }
        } else {
            // Argument completion - complete from current directory
            get_directory_completions(".", prefix, &completions, &count);
        }
    }
    
    // Sort completions alphabetically
    if (count > 0) {
        qsort(completions, count, sizeof(char*), compare_strings);
    }
    
    // Set output parameters
    *completions_ptr = completions;
    *num_completions_ptr = count;
}

/**
 * @brief Find the longest common prefix
 * 
 * Finds the longest common prefix among a set of strings.
 * 
 * @param strings Array of strings
 * @param count Number of strings
 * @return char* Longest common prefix (dynamically allocated, caller must free)
 */
char* find_longest_common_prefix(char **strings, int count) {
    if (count <= 0 || !strings || !strings[0]) {
        return strdup("");
    }
    
    // Start with the first string as potential LCP
    char *lcp = strdup(strings[0]);
    if (!lcp) {
        ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
        return NULL;
    }
    
    int lcp_len = strlen(lcp);
    
    // Compare with other strings and truncate LCP as needed
    for (int i = 1; i < count; i++) {
        int j = 0;
        while (j < lcp_len && strings[i][j] == lcp[j]) {
            j++;
        }
        
        // Truncate LCP to match length
        lcp[j] = '\0';
        lcp_len = j;
        
        // If LCP becomes empty, no need to continue
        if (lcp_len == 0) break;
    }
    
    return lcp;
}

/**
 * @brief Display all possible completions
 * 
 * Displays all possible completions when there are multiple matches.
 * 
 * @param completions Array of completions
 * @param num_completions Number of completions
 * @param input Current input buffer
 */
void display_completions(char **completions, int num_completions, const char *input) {
    if (!completions || num_completions <= 0) {
        return;
    }
    
    // Get terminal width
    int rows, cols;
    if (get_terminal_size(&rows, &cols) != 0) {
        cols = 80;  // Default terminal width
    }
    
    // Calculate the maximum width of a completion
    int max_width = 0;
    for (int i = 0; i < num_completions; i++) {
        int len = strlen(completions[i]);
        if (len > max_width) {
            max_width = len;
        }
    }
    max_width += 2;  // Add space between columns
    
    // Calculate number of columns and rows
    int num_cols = cols / max_width;
    if (num_cols == 0) num_cols = 1;
    
    int num_rows = (num_completions + num_cols - 1) / num_cols;
    
    // Print a newline and all completions
    printf("\n");
    
    for (int row = 0; row < num_rows; row++) {
        for (int col = 0; col < num_cols; col++) {
            int index = col * num_rows + row;
            if (index < num_completions) {
                printf("%-*s", max_width, completions[index]);
            }
        }
        printf("\n");
    }
    
    // Print prompt and input again
    printf("$ %s", input);
    fflush(stdout);
}

/**
 * @brief Free completions array
 * 
 * Frees memory allocated for completions.
 * 
 * @param completions Array of completions
 * @param num_completions Number of completions
 */
void free_completions(char **completions, int num_completions) {
    if (completions) {
        for (int i = 0; i < num_completions; i++) {
            free(completions[i]);
        }
        free(completions);
    }
}

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
                             char ***completions_ptr, int *num_completions_ptr) {
    if (!dir_path || !prefix || !completions_ptr || !num_completions_ptr) {
        return -1;
    }
    
    DIR *dir = opendir(dir_path);
    if (!dir) {
        ERROR_SYSTEM(ERR_IO, "Failed to open directory: %s", dir_path);
        return -1;
    }
    
    char **completions = *completions_ptr;
    int count = *num_completions_ptr;
    int capacity = count > 0 ? count * 2 : 10;
    
    // Make sure we have enough space
    if (count == 0) {
        completions = malloc(capacity * sizeof(char*));
        if (!completions) {
            ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
            closedir(dir);
            return -1;
        }
    }
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip . and .. unless specifically requested
        if (strcmp(prefix, ".") != 0 && strcmp(prefix, "..") != 0) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
        }
        
        // Check if entry matches prefix
        if (prefix_match(entry->d_name, prefix)) {
            // Create full path for stat
            char full_path[PATH_MAX];
            snprintf(full_path, PATH_MAX, "%s/%s", dir_path, entry->d_name);
            
            // Check if it's a directory
            struct stat st;
            if (stat(full_path, &st) == 0) {
                // Allocate space for completion (with trailing slash for directories)
                char *completion;
                if (S_ISDIR(st.st_mode)) {
                    // Directory - add trailing slash
                    completion = malloc(strlen(entry->d_name) + 2);
                    if (completion) {
                        strcpy(completion, entry->d_name);
                        strcat(completion, "/");
                    }
                } else {
                    // Regular file
                    completion = strdup(entry->d_name);
                }
                
                if (completion) {
                    // Check if we need to resize
                    if (count >= capacity) {
                        capacity *= 2;
                        char **new_completions = realloc(completions, capacity * sizeof(char*));
                        if (!new_completions) {
                            free(completion);
                            continue;
                        }
                        completions = new_completions;
                    }
                    
                    // Add to completions
                    completions[count++] = completion;
                }
            }
        }
    }
    
    closedir(dir);
    
    // Update output parameters
    *completions_ptr = completions;
    *num_completions_ptr = count;
    
    return 0;
}

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
int get_command_completions(const char *prefix, char ***completions_ptr, int *num_completions_ptr) {
    if (!prefix || !completions_ptr || !num_completions_ptr) {
        return -1;
    }
    
    char **completions = *completions_ptr;
    int count = *num_completions_ptr;
    int capacity = count > 0 ? count * 2 : 10;
    
    // Make sure we have enough space
    if (count == 0) {
        completions = malloc(capacity * sizeof(char*));
        if (!completions) {
            ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
            return -1;
        }
    }
    
    // Check built-in commands
    const BuiltinCommand *builtins = get_builtin_commands();
    for (int i = 0; builtins[i].name != NULL; i++) {
        if (prefix_match(builtins[i].name, prefix)) {
            // Add to completions array (resize if needed)
            if (count >= capacity) {
                capacity *= 2;
                char **new_completions = realloc(completions, capacity * sizeof(char*));
                if (!new_completions) {
                    continue;
                }
                completions = new_completions;
            }
            
            completions[count] = strdup(builtins[i].name);
            if (completions[count]) {
                count++;
            }
        }
    }
    
    // Check PATH for executables
    char *path_env = getenv("PATH");
    if (path_env) {
        char *path_copy = strdup(path_env);
        if (path_copy) {
            char *path = strtok(path_copy, ":");
            
            while (path != NULL) {
                DIR *dir = opendir(path);
                if (dir) {
                    struct dirent *entry;
                    while ((entry = readdir(dir)) != NULL) {
                        if (prefix_match(entry->d_name, prefix)) {
                            char full_path[PATH_MAX];
                            snprintf(full_path, PATH_MAX, "%s/%s", path, entry->d_name);
                            
                            struct stat st;
                            if (stat(full_path, &st) == 0 && 
                                S_ISREG(st.st_mode) && 
                                (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
                                
                                // Check for duplicates
                                int duplicate = 0;
                                for (int i = 0; i < count; i++) {
                                    if (strcmp(entry->d_name, completions[i]) == 0) {
                                        duplicate = 1;
                                        break;
                                    }
                                }
                                
                                if (!duplicate) {
                                    // Add to completions
                                    if (count >= capacity) {
                                        capacity *= 2;
                                        char **new_completions = realloc(completions, capacity * sizeof(char*));
                                        if (!new_completions) {
                                            continue;
                                        }
                                        completions = new_completions;
                                    }
                                    
                                    completions[count] = strdup(entry->d_name);
                                    if (completions[count]) count++;
                                }
                            }
                        }
                    }
                    closedir(dir);
                }
                path = strtok(NULL, ":");
            }
            free(path_copy);
        }
    }
    
    // Update output parameters
    *completions_ptr = completions;
    *num_completions_ptr = count;
    
    return 0;
}

/**
 * @brief Extract last word from input
 * 
 * Extracts the last word from the input string for completion.
 * 
 * @param input Input string
 * @return char* Last word (dynamically allocated, caller must free)
 */
char *extract_last_word(const char *input) {
    if (!input) {
        return NULL;
    }
    
    // Find last space
    const char *last_space = strrchr(input, ' ');
    const char *last_word;
    
    if (last_space) {
        // Last word starts after the last space
        last_word = last_space + 1;
    } else {
        // No spaces, the whole input is the last word
        last_word = input;
    }
    
    return strdup(last_word);
}

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
int add_completion(char ***completions, int *capacity, int *count, const char *completion) {
    if (!completions || !capacity || !count || !completion) {
        return -1;
    }
    
    // Check for duplicates
    for (int i = 0; i < *count; i++) {
        if (strcmp((*completions)[i], completion) == 0) {
            return 0;  // Already exists
        }
    }
    
    // Check if we need to resize
    if (*count >= *capacity) {
        *capacity *= 2;
        char **new_completions = realloc(*completions, *capacity * sizeof(char*));
        if (!new_completions) {
            ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
            return -1;
        }
        *completions = new_completions;
    }
    
    // Add the completion
    (*completions)[*count] = strdup(completion);
    if (!(*completions)[*count]) {
        ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
        return -1;
    }
    
    (*count)++;
    return 0;
}