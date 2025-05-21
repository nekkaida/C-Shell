/**
 * @file redirection.h
 * @brief I/O redirection component
 *
 * Functions for handling I/O redirection in commands.
 */

#ifndef REDIRECTION_H
#define REDIRECTION_H

#include "command.h"

/**
 * @brief Parse redirection operators
 * 
 * Extracts redirection operators from a command string.
 * 
 * @param input Command string (will be modified)
 * @param redir RedirectionInfo structure to fill
 * @return int 0 on success, non-zero on error
 */
int parse_redirection_operators(char *input, RedirectionInfo *redir);

/**
 * @brief Apply redirections
 * 
 * Sets up file descriptors according to redirection specification.
 * 
 * @param redir RedirectionInfo structure with redirection specification
 * @param original_fds Array to store original file descriptors (size 2)
 * @return int 0 on success, non-zero on error
 */
int apply_redirections(RedirectionInfo *redir, int *original_fds);

/**
 * @brief Restore original file descriptors
 * 
 * Restores file descriptors to their original state after redirection.
 * 
 * @param original_fds Array of original file descriptors (size 2)
 */
void restore_original_fds(int *original_fds);

/**
 * @brief Open redirection files
 * 
 * Opens files for redirection with appropriate flags.
 * 
 * @param redir RedirectionInfo structure with filenames
 * @param fds Array to store opened file descriptors (size 2)
 * @return int 0 on success, non-zero on error
 */
int open_redirection_files(RedirectionInfo *redir, int *fds);

/**
 * @brief Close redirection files
 * 
 * Closes files opened for redirection.
 * 
 * @param fds Array of file descriptors to close (size 2)
 */
void close_redirection_files(int *fds);

/**
 * @brief Duplicate file descriptor
 * 
 * Duplicates a file descriptor and closes the original.
 * 
 * @param oldfd Original file descriptor
 * @param newfd New file descriptor
 * @return int 0 on success, non-zero on error
 */
int duplicate_fd(int oldfd, int newfd);

#endif /* REDIRECTION_H */