#ifndef HEAP_MANAGEMENT_HELPER_H_
#define HEAP_MANAGEMENT_HELPER_H_

#include <stdint.h>

//
/**
 * @brief Helper functions for heap memory management.
 *
 * HeapManagementHelper provides functions for managing heap memory, including retrieving the amount of free memory.
 */
typedef struct {
    /**
     * @brief Retrieves the amount of free heap memory.
     *
     * This function calculates and returns the amount of free heap memory available.
     *
     * @return The amount of free heap memory in bytes.
     */
    int16_t (* getFreeMemory)(void);
} HeapManagementHelper;

/**
 * @brief Initializes the heap management helper.
 *
 * This function initializes the heap management helper and returns a pointer to the HeapManagementHelper structure.
 * To use the heap management helper, call the initialization function `dOS_initHeapManagementHelper` once in your program,
 * typically at the beginning or whenever heap memory management functionality is required.
 *
 * When heap memory management is no longer needed or to conserve memory, you can unload the heap management helper module
 * by freeing the allocated memory using the standard `free()` function.
 *
 * @note Make sure to check if the returned pointer is not NULL before using the functions.
 *
 * @return A pointer to the initialized HeapManagementHelper structure, or NULL if initialization fails.
 */
HeapManagementHelper * dOS_initHeapManagementHelper(void);

#endif /* HEAP_MANAGEMENT_HELPER_H_ */