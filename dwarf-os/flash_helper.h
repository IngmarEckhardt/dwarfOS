/**
 * @file flash_helper.h
 * @brief Flash memory management for DwarfOS.
 */

#ifndef DWARFOS_FLASH_HELPER_H
#define DWARFOS_FLASH_HELPER_H

#include <stdint.h>

/**
 * @brief Structure for managing text files in program memory less than 64KB.
 *
 * NearTextFile is used to manage text files that are stored in program memory and are less than 64KB in size.
 * These files can be generated with the `pgm_textfile_generator`.
 */
typedef struct {
    const uint8_t amountOfEntries; /**< The number of entries in the text file. */
    const uint8_t sizeOfIndexArray; /**< The size of the index array. */
    const uint16_t maxLengthOfStrings; /**< The maximum length of the strings in the text file. */
} NearTextFile;

#ifdef __AVR_HAVE_ELPM__
/**
 * @brief Structure for managing text files in extended program memory.
 *
 * FarTextFile is used to manage text files that are stored in extended program memory.
 * These files can be generated with the `pgm_textfile_generator`.
 */
typedef struct {
    const uint32_t farPointer; /**< The far pointer to the text file in extended program memory. */
    const uint8_t amountOfEntries; /**< The number of entries in the text file. */
    const uint8_t sizeOfIndexArray; /**< The size of the index array. */
    const uint16_t maxLengthOfStrings; /**< The maximum length of the strings in the text file. */
} FarTextFile;
#endif

/**
 * @brief Structure for managing string storage and lazy loading.
 *
 * FlashHelper manages the loading of strings from flash memory to RAM.
 * It contains functions to initialize string storage and load strings from flash memory.
 */
typedef struct {
    /**
   * @brief Loads a string from flash memory to RAM.
   *
   * This function takes a pointer to a string in flash memory and creates a copy of the string in RAM.
   * The function returns a pointer to the newly created string in RAM.
   *
   * @param flashString A pointer to the string in flash memory.
   * @return A pointer to the newly created string in RAM.
   */
    char * (* createString_P)(const char * flashString);

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
    int16_t (* compareString_P)(const char * string, const char * flashString);

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
    char * (* createFromFile_P)(NearTextFile * textFile, const uint8_t index);

    /**
    * @brief Loads a string from program memory to a buffer in RAM.
    *
    * This function takes a pointer to a buffer in RAM and a pointer to a string in program memory,
    * and loads the string from program memory to the buffer in RAM.
    *
    * @param stringBuffer A pointer to the buffer in RAM.
    * @param pointerToNearProgMemString A pointer to the string in program memory.
    */
    void (* loadString_P)(char * stringBuffer, const char * pointerToNearProgMemString);

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
    int16_t (* putFileString_P)(NearTextFile * textFile, const uint8_t index);

#ifdef __AVR_HAVE_ELPM__
    #ifdef __AVR_HAVE_ELPM__
    /**
     * @brief Loads a string from extended program memory to RAM.
     *
     * This function takes a far pointer to a string in extended program memory and creates a copy of the string in RAM.
     * The function returns a pointer to the newly created string in RAM.
     *
     * @param farFlashString A far pointer to the string in extended program memory.
     * @return A pointer to the newly created string in RAM.
     */
    char * (* createString_PF)(uint32_t farFlashString);

    /**
     * @brief Loads a string from extended program memory to a buffer in RAM.
     *
     * This function takes a pointer to a buffer in RAM and a far pointer to a string in extended program memory,
     * and loads the string from extended program memory to the buffer in RAM.
     *
     * @param stringBuffer A pointer to the buffer in RAM.
     * @param farFlashString A far pointer to the string in extended program memory.
     */
    void (* loadString_PF)(char * stringBuffer, uint32_t farFlashString);

    /**
     * @brief Loads a string from a text file in extended program memory to RAM.
     *
     * This function takes a pointer to a FarTextFile structure and an index, and loads the string
     * at the specified index from the text file in extended program memory to RAM. The function returns a pointer
     * to the newly created string in RAM.
     *
     * @param textFile A pointer to the FarTextFile structure.
     * @param index The index of the string in the text file.
     * @return A pointer to the newly created string in RAM.
     */
    char * (* createFromFile_PF)(FarTextFile * textFile, const uint8_t index);

    /**
     * @brief Writes a string from a text file in extended program memory to the standard output.
     *
     * This function takes a pointer to a FarTextFile structure and an index, and writes the string
     * at the specified index from the text file in extended program memory to the standard output. The function
     * returns 0 on success, or EOF on error.
     *
     * @param textFile A pointer to the FarTextFile structure.
     * @param index The index of the string in the text file.
     * @return 0 on success, or EOF on error.
     */
    int16_t (* putFileString_PF)(FarTextFile * textFile, const uint8_t index);
#endif
     uint32_t initMsg; /**< Initialization message. */
#else
    const char * initMsg; /**< Initialization message. */
#endif
} FlashHelper;

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
FlashHelper * dOS_initFlashHelper(void);

#endif //DWARFOS_FLASH_HELPER_H