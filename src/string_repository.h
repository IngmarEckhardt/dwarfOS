#ifndef STRINGS_REPO_H_
#define STRINGS_REPO_H_

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
int16_t getFreeMemory();



extern LazyLoadingString* arrayOfManagedLazyStringPointers[MAX_SIZE_STRING_DB];
char* loadStringFromFlash(const char* PROGMEM flashString);
#endif /* STRINGS_REPO_H_ */