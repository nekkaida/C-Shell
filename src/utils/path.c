/**
 * @file path.c
 * @brief Path manipulation implementation
 *
 * Implementation of path manipulation functions.
 */

#include "utils/path.h"
#include "utils/error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>
#include <libgen.h>
#include <errno.h>

/**
 * @brief Get directory name
 * 
 * Extracts the directory part of a path.
 * 
 * @param path Path to process
 * @return char* Directory part (caller must free)
 */
char *path_dirname(const char *path) {
    if (!path) {
        return NULL;
    }
    
    // Make a copy of the path since dirname might modify it
    char *path_copy = strdup(path);
    if (!path_copy) {
        ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
        return NULL;
    }
    
    char *dir = dirname(path_copy);
    char *result = strdup(dir);
    
    free(path_copy);
    return result;
}

/**
 * @brief Get base name
 * 
 * Extracts the file name part of a path.
 * 
 * @param path Path to process
 * @return char* File name part (caller must free)
 */
char *path_basename(const char *path) {
    if (!path) {
        return NULL;
    }
    
    // Make a copy of the path since basename might modify it
    char *path_copy = strdup(path);
    if (!path_copy) {
        ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
        return NULL;
    }
    
    char *base = basename(path_copy);
    char *result = strdup(base);
    
    free(path_copy);
    return result;
}

/**
 * @brief Join path components
 * 
 * Joins two path components with appropriate separators.
 * 
 * @param path1 First path component
 * @param path2 Second path component
 * @return char* Joined path (caller must free)
 */
char *path_join(const char *path1, const char *path2) {
    if (!path1 || !path2) {
        return NULL;
    }
    
    // Check for empty components
    if (*path1 == '\0') {
        return strdup(path2);
    }
    if (*path2 == '\0') {
        return strdup(path1);
    }
    
    // Calculate the length of the joined path
    size_t len1 = strlen(path1);
    size_t len2 = strlen(path2);
    int need_separator = (path1[len1 - 1] != '/' && path2[0] != '/');
    
    // Allocate memory for the joined path
    char *result = malloc(len1 + len2 + (need_separator ? 1 : 0) + 1);
    if (!result) {
        ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
        return NULL;
    }
    
    // Copy the first component
    strcpy(result, path1);
    
    // Add separator if needed
    if (need_separator) {
        result[len1] = '/';
        result[len1 + 1] = '\0';
    }
    
    // Append the second component
    strcat(result, path2);
    
    return result;
}

/**
 * @brief Normalize path
 * 
 * Normalizes a path (resolves ., .., and duplicate separators).
 * 
 * @param path Path to normalize
 * @return char* Normalized path (caller must free)
 */
char *path_normalize(const char *path) {
    if (!path) {
        return NULL;
    }
    
    // Allocate memory for the normalized path
    char *result = malloc(strlen(path) + 1);
    if (!result) {
        ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
        return NULL;
    }
    
    // Initialize result
    result[0] = '\0';
    
    // Split the path by separator
    char *path_copy = strdup(path);
    if (!path_copy) {
        ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
        free(result);
        return NULL;
    }
    
    // Handle absolute path
    int absolute = (path[0] == '/');
    if (absolute) {
        strcpy(result, "/");
    }
    
    // Tokenize and process path components
    char *token = strtok(path_copy, "/");
    char *result_end = result + strlen(result);
    
    while (token) {
        if (strcmp(token, ".") == 0) {
            // Skip "." components
        } else if (strcmp(token, "..") == 0) {
            // Handle ".." components - go up one level
            if (result_end > result + (absolute ? 1 : 0)) {
                // Find the last separator
                char *last_separator = strrchr(result, '/');
                if (last_separator && last_separator >= result + (absolute ? 1 : 0)) {
                    // Truncate at the last separator
                    *last_separator = '\0';
                    result_end = last_separator;
                } else if (!absolute) {
                    // Relative path and at the beginning - prepend "../"
                    size_t len = strlen(result);
                    memmove(result + 3, result, len + 1);
                    memcpy(result, "../", 3);
                    result_end += 3;
                }
            } else if (!absolute) {
                // Relative path and at the beginning - prepend "../"
                strcpy(result, "../");
                result_end += 3;
            }
        } else {
            // Regular component - append it
            if (result_end > result && result_end[-1] != '/') {
                *result_end++ = '/';
            }
            
            size_t token_len = strlen(token);
            memcpy(result_end, token, token_len);
            result_end += token_len;
            *result_end = '\0';
        }
        
        token = strtok(NULL, "/");
    }
    
    free(path_copy);
    
    // Handle empty result for relative path
    if (result[0] == '\0') {
        strcpy(result, ".");
    }
    
    return result;
}

/**
 * @brief Get absolute path
 * 
 * Converts a relative path to absolute path.
 * 
 * @param path Path to convert
 * @return char* Absolute path (caller must free)
 */
char *path_absolute(const char *path) {
    if (!path) {
        return NULL;
    }
    
    // Already absolute?
    if (path[0] == '/') {
        return strdup(path);
    }
    
    // Get current working directory
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        ERROR_SYSTEM(ERR_IO, "Failed to get current directory");
        return NULL;
    }
    
    // Join CWD and path
    char *joined = path_join(cwd, path);
    if (!joined) {
        return NULL;
    }
    
    // Normalize the result
    char *result = path_normalize(joined);
    free(joined);
    
    return result;
}

/**
 * @brief Check if path is absolute
 * 
 * @param path Path to check
 * @return int 1 if absolute, 0 if relative
 */
int path_is_absolute(const char *path) {
    if (!path) {
        return 0;
    }
    
    return path[0] == '/';
}

/**
 * @brief Expand path
 * 
 * Expands special characters (e.g., ~) in a path.
 * 
 * @param path Path to expand
 * @return char* Expanded path (caller must free)
 */
char *path_expand(const char *path) {
    if (!path) {
        return NULL;
    }
    
    // Check for ~ at the beginning
    if (path[0] == '~') {
        // Get home directory
        char *home = getenv("HOME");
        if (!home) {
            ERROR_ERROR(ERR_IO, "HOME environment variable not set");
            return strdup(path);
        }
        
        // Expand ~ to home directory
        if (path[1] == '\0') {
            // Just ~
            return strdup(home);
        } else if (path[1] == '/') {
            // ~/ - join home and rest
            return path_join(home, path + 2);
        }
    }
    
    // No expansion needed
    return strdup(path);
}

/**
 * @brief Get file extension
 * 
 * Extracts the file extension from a path.
 * 
 * @param path Path to process
 * @return char* File extension (caller must free)
 */
char *path_extension(const char *path) {
    if (!path) {
        return NULL;
    }
    
    // Find last component
    const char *base = strrchr(path, '/');
    if (base) {
        base++;
    } else {
        base = path;
    }
    
    // Find last dot in the last component
    const char *dot = strrchr(base, '.');
    if (!dot || dot == base) {
        // No extension
        return strdup("");
    }
    
    return strdup(dot + 1);
}

/**
 * @brief Check if path exists
 * 
 * @param path Path to check
 * @return int 1 if exists, 0 otherwise
 */
int path_exists(const char *path) {
    if (!path) {
        return 0;
    }
    
    struct stat st;
    return stat(path, &st) == 0;
}

/**
 * @brief Check if path is a directory
 * 
 * @param path Path to check
 * @return int 1 if directory, 0 otherwise
 */
int path_is_directory(const char *path) {
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
 * @brief Check if path is a regular file
 * 
 * @param path Path to check
 * @return int 1 if regular file, 0 otherwise
 */
int path_is_file(const char *path) {
    if (!path) {
        return 0;
    }
    
    struct stat st;
    if (stat(path, &st) != 0) {
        return 0;
    }
    
    return S_ISREG(st.st_mode);
}

/**
 * @brief Check if path is executable
 * 
 * @param path Path to check
 * @return int 1 if executable, 0 otherwise
 */
int path_is_executable(const char *path) {
    if (!path) {
        return 0;
    }
    
    return access(path, X_OK) == 0;
}

/**
 * @brief Get parent directory
 * 
 * Gets the parent directory of a path.
 * 
 * @param path Path to process
 * @return char* Parent directory (caller must free)
 */
char *path_parent(const char *path) {
    return path_dirname(path);
}

/**
 * @brief Search for file in PATH
 * 
 * Searches for a file in the directories specified by PATH.
 * 
 * @param filename File name to search for
 * @return char* Full path if found (caller must free), NULL otherwise
 */
char *path_search(const char *filename) {
    if (!filename) {
        return NULL;
    }
    
    // If filename contains a slash, it's a path - just check if it exists
    if (strchr(filename, '/') != NULL) {
        if (path_exists(filename) && path_is_executable(filename)) {
            return strdup(filename);
        }
        return NULL;
    }
    
    // Otherwise, search in PATH
    char *path_env = getenv("PATH");
    if (!path_env) {
        return NULL;
    }
    
    char *path_copy = strdup(path_env);
    if (!path_copy) {
        ERROR_ERROR(ERR_MEMORY, "Memory allocation error");
        return NULL;
    }
    
    char *path = strtok(path_copy, ":");
    char *result = NULL;
    
    while (path != NULL) {
        // Join path and filename
        char *full_path = path_join(path, filename);
        if (!full_path) {
            continue;
        }
        
        // Check if file exists and is executable
        if (path_exists(full_path) && path_is_executable(full_path)) {
            result = full_path;
            break;
        }
        
        free(full_path);
        path = strtok(NULL, ":");
    }
    
    free(path_copy);
    return result;
}