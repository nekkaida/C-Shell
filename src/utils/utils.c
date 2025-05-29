/**
 * @file memory.c
 * @brief Memory management implementation
 *
 * Implementation of memory management functions.
 */

#include "utils/memory.h"
#include "utils/error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Memory statistics (for debug builds)
#ifdef DEBUG
static size_t total_allocations = 0;
static size_t total_bytes_allocated = 0;
static size_t current_bytes_allocated = 0;
static size_t peak_bytes_allocated = 0;
#endif

/**
 * @brief Safe malloc
 * 
 * Allocates memory with error checking.
 * 
 * @param size Size to allocate
 * @return void* Allocated memory
 */
void *safe_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    
    void *ptr = malloc(size);
    
    if (!ptr) {
        ERROR_ERROR(ERR_MEMORY, "Failed to allocate %zu bytes", size);
        return NULL;
    }
    
#ifdef DEBUG
    total_allocations++;
    total_bytes_allocated += size;
    current_bytes_allocated += size;
    
    if (current_bytes_allocated > peak_bytes_allocated) {
        peak_bytes_allocated = current_bytes_allocated;
    }
#endif
    
    return ptr;
}

/**
 * @brief Safe calloc
 * 
 * Allocates and zeroes memory with error checking.
 * 
 * @param nmemb Number of elements
 * @param size Size of each element
 * @return void* Allocated memory
 */
void *safe_calloc(size_t nmemb, size_t size) {
    if (nmemb == 0 || size == 0) {
        return NULL;
    }
    
    // Check for overflow
    if (nmemb > SIZE_MAX / size) {
        ERROR_ERROR(ERR_MEMORY, "Integer overflow in calloc(%zu, %zu)", nmemb, size);
        return NULL;
    }
    
    void *ptr = calloc(nmemb, size);
    
    if (!ptr) {
        ERROR_ERROR(ERR_MEMORY, "Failed to allocate %zu bytes", nmemb * size);
        return NULL;
    }
    
#ifdef DEBUG
    total_allocations++;
    total_bytes_allocated += nmemb * size;
    current_bytes_allocated += nmemb * size;
    
    if (current_bytes_allocated > peak_bytes_allocated) {
        peak_bytes_allocated = current_bytes_allocated;
    }
#endif
    
    return ptr;
}

/**
 * @brief Safe realloc
 * 
 * Reallocates memory with error checking.
 * 
 * @param ptr Pointer to reallocate
 * @param size New size
 * @return void* Reallocated memory
 */
void *safe_realloc(void *ptr, size_t size) {
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    
    // If ptr is NULL, equivalent to malloc
    if (!ptr) {
        return safe_malloc(size);
    }
    
#ifdef DEBUG
    // Get the old size - not possible in standard C without tracking
    // For now, just count as a new allocation
    total_allocations++;
    total_bytes_allocated += size;
    current_bytes_allocated += size;
    
    if (current_bytes_allocated > peak_bytes_allocated) {
        peak_bytes_allocated = current_bytes_allocated;
    }
#endif
    
    void *new_ptr = realloc(ptr, size);
    
    if (!new_ptr) {
        ERROR_ERROR(ERR_MEMORY, "Failed to reallocate %zu bytes", size);
        return NULL;
    }
    
    return new_ptr;
}

/**
 * @brief Safe free
 * 
 * Frees memory and sets pointer to NULL.
 * 
 * @param pptr Pointer to pointer to free
 */
void safe_free(void **pptr) {
    if (!pptr || !*pptr) {
        return;
    }
    
#ifdef DEBUG
    // No way to know the size of the allocation being freed in standard C
#endif
    
    free(*pptr);
    *pptr = NULL;
}

/**
 * @brief Duplicate memory block
 * 
 * Duplicates a memory block.
 * 
 * @param src Source pointer
 * @param size Size to duplicate
 * @return void* Duplicated memory
 */
void *mem_dup(const void *src, size_t size) {
    if (!src || size == 0) {
        return NULL;
    }
    
    void *dest = safe_malloc(size);
    if (!dest) {
        return NULL;
    }
    
    memcpy(dest, src, size);
    return dest;
}

/**
 * @brief Allocate memory for string of specified length
 * 
 * @param len String length (not including null terminator)
 * @return char* Allocated memory
 */
char *str_alloc(size_t len) {
    return (char *)safe_malloc(len + 1);
}

/**
 * @brief Debug memory allocation
 * 
 * Allocates memory with debugging information.
 * 
 * @param size Size to allocate
 * @param file Source file
 * @param line Line number
 * @return void* Allocated memory
 */
void *debug_malloc(size_t size, const char *file, int line) {
    void *ptr = safe_malloc(size);
    
    if (ptr) {
        ERROR_DEBUG(file, line, "debug_malloc", "Allocated %zu bytes at %p", size, ptr);
    }
    
    return ptr;
}

/**
 * @brief Print memory statistics
 * 
 * Prints memory allocation statistics (debug build only).
 */
void print_memory_stats(void) {
#ifdef DEBUG
    printf("Memory statistics:\n");
    printf("  Total allocations: %zu\n", total_allocations);
    printf("  Total bytes allocated: %zu\n", total_bytes_allocated);
    printf("  Current bytes allocated: %zu\n", current_bytes_allocated);
    printf("  Peak bytes allocated: %zu\n", peak_bytes_allocated);
#else
    printf("Memory statistics only available in debug build\n");
#endif
}