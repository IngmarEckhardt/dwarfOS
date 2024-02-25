#ifndef DWARFOS_STRING_REPOSITORY_H
#define DWARFOS_STRING_REPOSITORY_H
#include <stdint.h>


#define MAX_SIZE_STRING_DB 8 // Number of maximum Strings in DB, 2^n is recommended


extern char* __brkval;


/**
 * @brief Structure to manage lazy loading of strings from flash program memory
 *
 * FlashString holds the reference to the address of the string in the flash program memory, pointerToString holds a
 * reference to the heap address of the string if it was already loaded into the heap, or is NULL if it wasn't loaded
 * before.
 *
 * It's always necessary to use at least one variable to a string, a pointer to a string, that you will use in your
 * program this construct add another pointer, so the use of RAM(stack of main) will increase by 2Byte for a pointer at
 * avr-gcc compiler for every Lazy Loading String you manage here, but it will spare you space in RAM at startup for
 * every String that is larger than this 2byte.
 *
 * It only uses the RAM if the String is needed, and then uses the heap. You are free to free the heap memory for this
 * string (pointerToString) manually or add the string to the array and use the rudimentary memory management that aims
 * as priority for a low overhead of management
 * */
typedef struct {
    const char* flashString;
    char* pointerToString;
} LazyLoadingString;


LazyLoadingString** addString(LazyLoadingString* stringToAdd);

char* getString(LazyLoadingString* stringToFetch);

LazyLoadingString* freeString(LazyLoadingString* stringToKill);

void freeMemoryRandom(uint8_t percentage);

int16_t getFreeMemory(void);

LazyLoadingString* removeStringFromManagement(LazyLoadingString* stringToKill);


//debugging purpose
extern LazyLoadingString* arrayOfManagedLazyStringPointers[MAX_SIZE_STRING_DB];



#endif /* DWARFOS_STRING_REPOSITORY_H */