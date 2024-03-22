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
 * FarTextFile is used to manage text files that are stored in extended program memory.
 * These files can be generated with the `pgm_textfile_generator`.
 */
typedef struct {
    const uint32_t pointerToArray; /**< The far pointer to the text file in extended program memory. */
    const uint8_t amountOfEntries; /**< The number of entries in the text file. */
    const uint8_t sizeOfIndexArray; /**< The size of the index array. */
    const uint16_t maxLengthOfStrings; /**< The maximum length of the strings in the text file. */
} TextFile;

typedef struct FlashHelper FlashHelper;
/**
 * @brief Structure for managing string storage and lazy loading.
 *
 * FlashHelper manages the loading of strings from flash memory to RAM.
 * It contains functions to initialize string storage and load strings from flash memory.
 */
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
    * @brief Loads a string from program memory to a buffer in RAM.
    *
    * This function takes a pointer to a buffer in RAM and a pointer to a string in program memory,
    * and loads the string from program memory to the buffer in RAM.
    *
    * @param stringBuffer A pointer to the buffer in RAM.
    * @param pointerToNearProgMemString A pointer to the string in program memory.
    */
    char * (* copyString_P)(char * stringBuffer, uint32_t flashString);

    int16_t (*putString_P)(uint32_t farPointerToString);

    uint16_t (* lengthOfString_P)(uint32_t flashString);

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
    * @brief Loads a string from a text file in program memory to RAM.
    *
    * This function takes a pointer to a NearTextFile structure and an index, and loads the string
    * at the specified index from the text file in program memory to RAM. The function returns a pointer
    * to the newly created string in RAM.
    *
    * @param textFile A pointer to the NearTextFile structure.
    * @param index The index of the string in the text file.
    * @return A pointer to the newly created string in RAM.
    */
    char * (* createFileString_P)(TextFile * textFile, const uint8_t index, FlashHelper * helper);


    /**
    * @brief Writes a string from a text file in program memory to the standard output.
    *
    * This function takes a pointer to a NearTextFile structure and an index, and writes the string
    * at the specified index from the text file in program memory to the standard output. The function
    * returns 0 on success, or EOF on error.
    *
    * @param textFile A pointer to the NearTextFile structure.
    * @param index The index of the string in the text file.
    * @return 0 on success, or EOF on error.
    */
    int16_t (* putFileString_P)(TextFile * textFile, const uint8_t index, FlashHelper * helper);

//if desired String in Ram is 1 the function returns the string, otherwise it puts the string into stdout
    char * (*getOrPutDosMessage)(uint8_t message, uint8_t desiredStringInRam, FlashHelper * helper);
};

/**
 * @brief Initializes the string storage.
 *
 * This function initializes the string storage and returns a pointer to the FlashHelper structure.
 * The storage exists in the heap. You can use it as a module and load it into the RAM if needed, or unload it
 * with free() if you require the memory.
 *
 * @note Make sure to check if the returned pointer is not NULL before using the functions.
 * @return A pointer to the initialized FlashHelper structure.
 */
FlashHelper * dOS_initFlashHelper(uint8_t desiredState);

#ifdef __AVR_HAVE_ELPM__
#define addressOf(flashString) pgm_get_far_address(flashString)
#else
#define addressOf(flashString) ((uint32_t) &flashString)
#endif

#define IDENT_STRING 0
#define FREE_MEMORY_STRING 1
#define FATAL_ERROR_STRING 2

#endif //DWARFOS_FLASH_HELPER_H