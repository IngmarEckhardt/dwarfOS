/**
 * @file flash_helper.h
 * @brief Flash memory management for DwarfOS.
 */

#ifndef DWARFOS_FLASH_HELPER_H
#define DWARFOS_FLASH_HELPER_H


#include <stdint.h>


/**
 * @brief Structure for managing text files in extended program memory.
 *
 * TextFile is used to manage text files that are stored in program memory.
 * These files can be generated with the `pgm_textfile_generator`.
 */
typedef struct {
    const uint32_t pointerToArray; /**< The pointer to the text file in program memory. */
    const uint8_t amountOfEntries; /**< The number of entries in the text file. */
    const uint8_t sizeOfIndexArray; /**< The size of the index array. */
    const uint16_t maxLengthOfStrings; /**< The maximum length of the strings in the text file. */
} TextFile;

/**
 * @brief Structure for managing string storage
 *
 * FlashHelper manages the loading of strings from flash memory to RAM.
 * It contains function pointers to load strings from flash memory.
 * The actual functions pointed to by these pointers can vary depending on whether the device supports
 * Extended Load Program Memory (ELPM). This is determined at startup.
 */
typedef struct FlashHelper FlashHelper;
struct FlashHelper {
    /**
   * @brief Loads a string from flash memory to RAM.
   *
   * This function takes a pointer to a string in flash memory and creates a copy of the string in RAM.
   * The function returns a pointer to the newly created string in RAM.
   *
   * @param flashString A pointer to the string in flash memory.
   * @return A pointer to the newly created string in RAM.
   */
    char * (* createString_P)(uint32_t flashString, FlashHelper * helper);

    /**
    * @brief Loads a string from program memory to a stdoutCopyBuffer in RAM.
    *
    * This function takes a pointer to a stdoutCopyBuffer in RAM and a pointer to a string in program memory,
    * and loads the string from program memory to the stdoutCopyBuffer in RAM.
    *
    * @param stringBuffer A pointer to the stdoutCopyBuffer in RAM.
    * @param pointerToProgMemString A pointer to the string in program memory.
    */
    char * (* copyString_P)(char * stringBuffer, uint32_t flashString);

    /**
    * @brief Writes a string from program memory to the standard output.
    *
    * This function takes a pointer to a string in program memory and writes the string to the standard output.
    * The function returns 0 on success, or EOF on error.
    *
    * @param pointerToString A pointer to the string in program memory.
    * @return 0 on success, or EOF on error.
    */
    int16_t (* putString_P)(uint32_t pointerToString);

    /**
    * @brief Returns the length of a string in program memory.
    *
    * This function takes a pointer to a string in program memory and returns the length of the string.
    *
    * @param flashString A pointer to the string in program memory.
    * @return The length of the string.
    */
    uint16_t (* lengthOfString_P)(uint32_t flashString);

    /**
    * @brief Reads a byte from program memory.
    *
    * This function takes an address in program memory and returns the byte at that address.
    *
    * @param address The address in program memory.
    * @return The byte at the specified address.
    */
    uint8_t (* readByte_P)(uint32_t address);

    /**
    * @brief Compares a string in RAM with a string in flash memory.
    *
    * This function takes a pointer to a string in RAM and a pointer to a string in flash memory,
    * and compares the two strings. The function returns 0 if the strings are equal, and a non-zero
    * value if the strings are not equal.
    *
    * @param string A pointer to the string in RAM.
    * @param flashString A pointer to the string in flash memory.
    * @return 0 if the strings are equal, and a non-zero value if the strings are not equal.
    */
    int16_t (* compareString_P)(const char * string, uint32_t flashString);

    /**
    * @brief Retrieves a string from a text file in program memory and loads it into RAM.
    *
    * This function takes a pointer to a TextFile structure and an index. If the TextFile has an index array (sizeOfIndexArray is not 0),
    * it finds the string in the text file where the stored number in the index array matches the provided index. If the TextFile does not
    * have an index array (sizeOfIndexArray is 0), it retrieves the string at the position corresponding to the provided index.
    * The retrieved string is then loaded into RAM and the function returns a pointer to this newly created string.
    *
    * @param textFile A pointer to the TextFile structure.
    * @param index The index of the string in the text file or the number to match in the index array.
    * @return A pointer to the newly created string in RAM.
    */
    char * (* createFileString_P)(TextFile * textFile, const uint8_t index, FlashHelper * helper);

    /**
    * @brief Writes a string from a text file in program memory to the standard output.
    *
    * This function takes a pointer to a TextFile structure and an index. If the TextFile has an index array (sizeOfIndexArray is not 0),
    * it finds the string in the text file where the stored number in the index array matches the provided index. If the TextFile does not
    * have an index array (sizeOfIndexArray is 0), it retrieves the string at the position corresponding to the provided index.
    * The retrieved string is then written to the standard output. The function returns 0 on success, or EOF on error.
    *
    * @param textFile A pointer to the TextFile structure.
    * @param index The index of the string in the text file or the number to match in the index array.
    * @return 0 on success, or EOF on error.
    */
    int16_t (* putFileString_P)(TextFile * textFile, const uint8_t index, FlashHelper * helper);
    /**
    * @brief Retrieves or prints a specific message from program memory.
    *
    * This function takes a message identifier, a flag indicating whether the string should be returned or printed,
    * and a pointer to a FlashHelper structure. If the flag is set (1), the function retrieves the corresponding
    * message from program memory and returns it as a string in RAM. If the flag is not set (0), the function prints
    * the corresponding message from program memory to the standard output.
    *
    * @param message An identifier for the message to be retrieved or printed. This corresponds to the macros defined at the end of the file.
    * @param desiredStringInRam A flag indicating whether the message should be returned as a string in RAM (1) or printed to the standard output (0).
    * @param helper A pointer to the FlashHelper structure.
    * @return If desiredStringInRam is set, a pointer to the newly created string in RAM. If desiredStringInRam is not set, NULL.
    */
    char * (* getOrPutDosMessage)(uint8_t message, uint8_t desiredStringInRam, FlashHelper * helper);
};

/**
 * @brief Initializes the string storage.
 *
 * This function initializes the string storage and returns a pointer to the FlashHelper structure.
 * The storage exists in the heap. You can use it as a module and load it into the RAM if needed, or unload it
 * with free() if you require the memory.
 *
 * @note Make sure to check if the returned pointer is not NULL before using the functions.
 * @param desiredState Specifies the type of Getters to use. Options are:
 *                     0 - Automatic decision between ELPM or non-ELPM
 *                     1 - Use non-ELPM Getters
 *                     2 - Use ELPM Getters*
 * @return A pointer to the initialized FlashHelper structure.
 */
FlashHelper * dOS_initFlashHelper(uint8_t desiredState);

#ifdef __AVR_HAVE_ELPM__

// the user need the reference here to get access to the pgm_get_far_address macro, but for easier testing its not included per default

#include <avr/pgmspace.h>
/**
 * @brief Macro to get the address of a string in program memory.
 *
 * This macro takes a string in program memory and returns its address. The address is returned as a 32-bit integer.
 * The macro uses different methods to get the address depending on whether the device supports Extended Load Program Memory (ELPM).
 *
 * @param flashString The string in program memory.
 * @return The address of the string.
 */
#define addressOf(flashString) pgm_get_far_address(flashString)
#else
/**
 * @brief Macro to get the address of a string in program memory.
 *
 * This macro takes a string in program memory and returns its address. The address is returned as a 32-bit integer.
 * The macro uses different methods to get the address depending on whether the device supports Extended Load Program Memory (ELPM).
 *
 * @param flashString The string in program memory.
 * @return The address of the string.
 */
#define addressOf(flashString) ((uintptr_t) &flashString)
#endif

/*
 * @brief Macros for identifying specific strings in program memory.
 *
 * These macros are used to identify specific strings in a program memory array. The strings are used for various purposes in the program.
 */

#define IDENT_STRING 0
#define FREE_MEMORY_STRING 1
#define FATAL_ERROR_STRING 2
#define TIMESTAMP_STRING_NUMBER_LF_FORMATSTRING 3

#endif //DWARFOS_FLASH_HELPER_H