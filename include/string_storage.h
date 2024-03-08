#ifndef DWARFOS_STRING_STORAGE_H
#define DWARFOS_STRING_STORAGE_H

#include <stdint.h>

typedef struct {
    const char * flashString; // Pointer to the string in flash memory
    char * pointerToString; // Pointer to the string in RAM
} LazyLoadingString;
/**
 * @brief Structure for managing string storage and lazy loading.
 *
 * StringStorage manages the loading of strings from flash memory to RAM.
 * It contains functions to initialize string storage and load strings from flash memory.
 */
typedef struct {
    LazyLoadingString initMsg; // Initialization message

    /**
    * @brief Loads a string from flash memory.
    *
    * This function loads a string from flash memory to RAM.
    *
    * @param flashString Pointer to the string in flash memory.
    * @return A pointer to the loaded string.
    */
    char * (* createStringFromFlash)(const char * flashString);

    void (* loadFromFlash)(char * stringBuffer, const char * flashString);

    uint8_t (* readProgMemByte)(const uint8_t * addressOfByte);
} StringStorage;

/**
 * @brief Initializes the string storage.
 *
 * This function initializes the string storage and returns a pointer to the StringStorage structure.
 * The storage exists in the heap. You can use it as a module and load it into the RAM if needed, or unload it
 * with free() if you require the memory.
 *
 * @note Make sure to check if the returned pointer is not NULL before using the functions.
 * @return A pointer to the initialized StringStorage structure.
 */
StringStorage * dOS_initStringStorage(void);


#endif //DWARFOS_STRING_STORAGE_H