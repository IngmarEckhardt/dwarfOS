#ifndef DWARFOS_FLASH_HELPER_H
#define DWARFOS_FLASH_HELPER_H

#include <stdint.h>
// ToDo Documentation of Functions

// managment of arrays in <64kb Progmem, can be generated with pgm_testfile_generator
typedef struct {
    char * pointerToNearProgMemString;
    const uint8_t amountOfEntries;
    const uint8_t sizeOfIndexArray;
    const uint16_t maxLengthOfStrings;
} NearTextFile;
// same for far progmem
#ifdef __AVR_HAVE_ELPM__
typedef struct {
    const uint32_t farPointer;
    const uint8_t amountOfEntries;
    const uint8_t sizeOfIndexArray;
    const uint16_t maxLengthOfStrings;
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
    * @brief Loads a string from flash memory.
    *
    * This function loads a string from flash memory to RAM.
    *
    * @param flashString Pointer to the string in flash memory.
    * @return A pointer to the loaded string.
    */
    char * (* createStringFromFlash)(const char * flashString);

    uint16_t (* readNearWord)(const uint16_t * intAdress);

    int16_t (* compareWithFlashString)(const char * string, const char * flashString);

    char * (* loadNearStringFromFile)(NearTextFile * textFile, const uint8_t index);

    void (* loadNearStringFromFlash)(char * stringBuffer, const char * pointerToNearProgMemString);

#ifdef __AVR_HAVE_ELPM__
    char * (* createFarStringFromFlash)(uint32_t farFlashString); // uint_farptr_t == uint32_t

    void (* loadFarStringFromFlash)(char * stringBuffer, uint32_t farFlashString);

    char * (* loadFarStringFromFile)(FarTextFile * textFile, const uint8_t index);

    uint32_t initMsg; // Initialization message
#else
    const char * initMsg;
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