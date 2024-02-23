#include <string_repository.h>

uint8_t getHash(LazyLoadingString* stringToAdd);

int8_t findStringInDb(LazyLoadingString* stringToFetch);

LazyLoadingString* arrayOfManagedLazyStringPointers[MAX_SIZE_STRING_DB] = {};


LazyLoadingString** addString(LazyLoadingString* stringToAdd) {

    uint8_t placement = getHash(stringToAdd);
    for (int i = 0; i < MAX_SIZE_STRING_DB; i++) {
        placement = (placement + i) % MAX_SIZE_STRING_DB;
        if (arrayOfManagedLazyStringPointers[placement] == NULL) {
            arrayOfManagedLazyStringPointers[placement] = stringToAdd;
            return &arrayOfManagedLazyStringPointers[placement];
        }
    }
    return NULL;
}

char* getString(LazyLoadingString* stringToFetch) {
    if (stringToFetch->pointerToString == NULL) {
        stringToFetch->pointerToString = loadStringFromFlash(stringToFetch->flashString);
    }
    return stringToFetch->pointerToString;
}

LazyLoadingString* freeString(LazyLoadingString* stringToKill) {
    free(stringToKill->pointerToString);
    stringToKill->pointerToString = NULL;
    return stringToKill;
}

LazyLoadingString* removeStringFromManagement(LazyLoadingString* stringToKill) {

    int8_t index = findStringInDb(stringToKill);
    if (index >= 0) {
        freeString(stringToKill);
        arrayOfManagedLazyStringPointers[index] = NULL;
        return stringToKill;
    }
    return NULL;
}


void freeMemoryRandom(uint8_t percentage) {

    int moduloValue = (percentage * MAX_SIZE_STRING_DB) / 100;

    for (int i = 0; i < MAX_SIZE_STRING_DB; i++) {
        if (i % moduloValue == 0) {
            freeString(arrayOfManagedLazyStringPointers[i]);
        }
    }
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


int16_t getFreeMemory(void) {
    return __brkval ? ((int16_t) SP) - ((int16_t) __brkval) : ((int16_t) SP) - ((int16_t) &__malloc_heap_start);
}


int8_t findStringInDb(LazyLoadingString* stringToFetch) {
    uint8_t placement = getHash(stringToFetch);
    for (int i = 0; i < MAX_SIZE_STRING_DB; i++) {
        placement = (placement + i) % MAX_SIZE_STRING_DB;
        if (arrayOfManagedLazyStringPointers[placement] == stringToFetch) {

            if (arrayOfManagedLazyStringPointers[placement] == stringToFetch) {
                return (int8_t) placement;
            }
        }
    }
    return -1;
}