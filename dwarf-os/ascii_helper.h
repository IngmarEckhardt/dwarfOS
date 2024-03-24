#ifndef DWARFOS_ASCII_HELPER_H
#define DWARFOS_ASCII_HELPER_H

#include "stdint.h"


/**
 * @brief Helper functions for ASCII operations.
 *
 * AsciiHelper provides functions for converting integers to ASCII and concatenating strings.
 */
typedef struct {
    /**
     * @brief Converts an integer to ASCII representation.
     *
     * This function converts the given integer to its ASCII representation and stores it in the provided stdoutCopyBuffer.
     * The size parameter specifies the maximum size of the result stdoutCopyBuffer, and the position parameter specifies
     * the position within the stdoutCopyBuffer to start writing the ASCII representation.
     *
     * @param result The stdoutCopyBuffer to store the ASCII representation.
     * @param num The integer to convert to ASCII.
     * @param size The maximum size of the result stdoutCopyBuffer.
     * @param position The position within the stdoutCopyBuffer to start writing the ASCII representation.
     */
    void (* integerToAscii)(char * result, uint32_t num, uint8_t size, uint8_t position);
    /**
     * @brief Concatenates multiple strings into a single string.
     *
     * This function concatenates the specified number of strings into a single string.
     * The amountOfStrings parameter specifies the number of strings to concatenate,
     * and the strings parameter is an array of pointers to the strings to concatenate.
     *
     * @param amountOfStrings The number of strings to concatenate.
     * @param strings An array of pointers to the strings to concatenate.
     * @return A pointer to the concatenated string.
     */
    char * (* concatStrings)(uint8_t amountOfStrings, char * strings[]);
} AsciiHelper;

/**
 * @brief Initializes the ASCII helper.
 *
 * This function initializes the ASCII helper and returns a pointer to the AsciiHelper structure.
 * The ASCII helper provides functions for converting integers to ASCII representation and concatenating strings.
 *
 * To use the ASCII helper, call this initialization function once in your program, typically at the beginning or whenever
 * ASCII helper functionality is required.
 *
 * When ASCII helper functionality is no longer needed or to conserve memory, you can unload the ASCII helper module by
 * freeing the allocated memory using the standard `free()` function.
 *
 * @note Make sure to check if the returned pointer is not NULL before using the ASCII helper functions.
 *
 * @return A pointer to the initialized AsciiHelper structure, or NULL if initialization fails.
 *
 */
AsciiHelper * dOS_initAsciiHelper(void);


#endif //DWARFOS_ASCII_HELPER_H
