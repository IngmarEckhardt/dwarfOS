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

typedef struct {
    LazyLoadingString** arrayOfManagedLazyStrings;

    LazyLoadingString** (* addString)(LazyLoadingString* stringToAdd);

    char* (* getString)(LazyLoadingString* stringToFetch);

    int16_t (* freeMemoryUpTo)(int16_t minAcceptedFreeMemByte, int16_t freeMemoryUpToByte);

    int16_t (* getFreeMemory)();
} DwarfDB;

extern const DwarfDB dwarfDB;
#endif /* STRINGS_REPO_H_ */