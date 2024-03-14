#ifndef DWARFOS_FLASH_HELPER_H
#define DWARFOS_FLASH_HELPER_H

#include <stdint.h>

typedef struct {
    const char * flashString; // Pointer to the string in flash memory
    char * pointerToString; // Pointer to the string in RAM
} LazyLoadingString;
/**
 * @brief Structure for managing string storage and lazy loading.
 *
 * FlashHelper manages the loading of strings from flash memory to RAM.
 * It contains functions to initialize string storage and load strings from flash memory.
 */
typedef struct {
    uint32_t initMsg; // Initialization message

    /**
    * @brief Loads a string from flash memory.
    *
    * This function loads a string from flash memory to RAM.
    *
    * @param flashString Pointer to the string in flash memory.
    * @return A pointer to the loaded string.
    */
    char * (* createStringFromFlash)(const char * flashString);
    char * (* createFarStringFromFlash)(uint32_t farFlashString); // uint_farptr_t == uint32_t

    void (* loadStringFromFlash)(char * stringBuffer, const char * flashString);
    void (* loadFarStringFromFlash)(char * stringBuffer,  uint32_t farFlashString);

    uint8_t (* readProgMemByte)(const uint8_t * addressOfByte);
    uint8_t (* readFarProgMemByte)(uint32_t addressOfByte);

    uint16_t (*readNearWord)(const uint16_t * intAdress);
    uint16_t (*readFarWord)(uint32_t intAdress);

    int16_t (*compareWithFlashString)(const char * string, const char * flashString);
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