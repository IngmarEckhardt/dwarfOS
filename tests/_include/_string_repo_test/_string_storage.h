#ifndef DWARFOS_STRING_STORAGE_H
#define DWARFOS_STRING_STORAGE_H


typedef struct {
    const char * flashString;
    char * pointerToString;
} LazyLoadingString;
typedef struct {
    LazyLoadingString initMsg;

    /**
    * @brief Loads a string from flash memory.
    *
    * This function loads a string from flash memory.
    *
    * @param flashString Pointer to the string in flash memory.
    * @return A pointer to the loaded string.
    */
    char * (* loadStringFromFlash)(const char * flashString);
} StringStorage;

StringStorage * dOS_initStringStorage(void);



#endif //DWARFOS_STRING_STORAGE_H