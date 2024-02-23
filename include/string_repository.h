#ifndef CAT_FOUNTAIN_STRING_REPOSITORY_H
#define CAT_FOUNTAIN_STRING_REPOSITORY_H

#include <avr/pgmspace.h>
#include <stdlib.h>

#define MAX_SIZE_STRING_DB 8 // Number of maximum Strings in DB, 2^n is recommended

extern char* __brkval;

typedef struct {
    const char* flashString;
    char* pointerToString;
} LazyLoadingString;


LazyLoadingString** addString(LazyLoadingString* stringToAdd);

char* getString(LazyLoadingString* stringToFetch);

LazyLoadingString* freeString(LazyLoadingString* stringToKill);

void freeMemoryRandom(uint8_t percentage);

LazyLoadingString* removeStringFromManagement(LazyLoadingString* stringToKill);

int16_t getFreeMemory(void);


//debugging purpose
extern LazyLoadingString* arrayOfManagedLazyStringPointers[MAX_SIZE_STRING_DB];

char* loadStringFromFlash(const char* PROGMEM flashString);

#endif /* CAT_FOUNTAIN_STRING_REPOSITORY_H */