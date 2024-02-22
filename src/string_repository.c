#include "string_repository.h"

char* loadStringFromFlash(const char* PROGMEM flashString);

LazyLoadingString** addString(LazyLoadingString* stringToAdd);

char* getString(LazyLoadingString* stringToFetch);

int16_t freeMemoryUpTo(int16_t minAcceptedFreeMem, int16_t freeMemoryUpTo);

int16_t getFreeMemory();

uint8_t getHash(LazyLoadingString* stringToAdd);

LazyLoadingString* arrayOfManagedLazyStrings[MAX_SIZE_STRING_DB] = {};
const DwarfDB dwarfDB = {arrayOfManagedLazyStrings, addString, getString, freeMemoryUpTo, getFreeMemory};

LazyLoadingString** addString(LazyLoadingString* stringToAdd) {

    uint8_t placement = getHash(stringToAdd);
    for (int i = 0; i < MAX_SIZE_STRING_DB; i++) {
        placement = (placement + i) % MAX_SIZE_STRING_DB;
        if (dwarfDB.arrayOfManagedLazyStrings[placement] == NULL) {
            dwarfDB.arrayOfManagedLazyStrings[placement] = stringToAdd;
            return &dwarfDB.arrayOfManagedLazyStrings[placement];
        }
    }
    return NULL;
}

char* getString(LazyLoadingString* stringToFetch) {

    uint8_t placement = getHash(stringToFetch);
    for (int i = 0; i < MAX_SIZE_STRING_DB; i++) {
        placement = (placement + i) % MAX_SIZE_STRING_DB;
        if (dwarfDB.arrayOfManagedLazyStrings[placement] == stringToFetch) {
            if (stringToFetch->pointerToString == NULL) {
                stringToFetch->pointerToString = loadStringFromFlash(stringToFetch->flashString);
            }
            return stringToFetch->pointerToString;
        }
    }
    return NULL;
}

int16_t freeMemoryUpTo(int16_t minAcceptedFreeMem, int16_t freeMemoryUpTo) {
    uint8_t trials = 1;
    int16_t freeMemory = getFreeMemory();
    if (freeMemory < minAcceptedFreeMem) {
        while (freeMemory < freeMemoryUpTo && trials) {
            //make sure the value change with taking increasing trials into account
            free(dwarfDB.arrayOfManagedLazyStrings[(freeMemory + trials) % MAX_SIZE_STRING_DB]->pointerToString);
            freeMemory = getFreeMemory();
            trials++;
        } //trials will overflow to zero after 254 attempts
    }
    return freeMemory;
}

//Hashing will only work until 255 managed strings
uint8_t getHash(LazyLoadingString* stringToAdd) {
    return (((uint16_t) stringToAdd) % MAX_SIZE_STRING_DB);
}

char* loadStringFromFlash(const char* PROGMEM flashString) {
    uint16_t length = strlen_P(flashString) + 1;
    char* result;
    result = (char*) calloc(length, 1);
    if (result == NULL) {
        return NULL;
    }
    strcpy_P(result, flashString);
    return result;
}


int16_t getFreeMemory() {
    return __brkval ? ((int16_t) SP) - ((int16_t) __brkval) : ((int16_t) SP) - ((int16_t) &__malloc_heap_start);
}