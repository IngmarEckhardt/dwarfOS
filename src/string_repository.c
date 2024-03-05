#include <string_repository.h>
#include <stdlib.h>

// Hashing function (limited to 255 managed strings)
uint8_t getHash(LazyLoadingString * stringToAdd, uint8_t size);

// Find a string in the managed database, has to be int16_t to return -1 as fail, and hold all positive values for uint8
int16_t findStringInDb(LazyLoadingString * stringToFetch, LazyLoadingString ** arrayOfManagedLazyStringPointers, uint8_t size);


// Add a string to the database of managed strings, enabling lazy loading and availability for freeMemoryRandom function.
// Alternatively, users can manage this manually using the data structures.
// uses placement with hashing, best case O(1), could be changed to placement without hashing for small amount of strings
LazyLoadingString ** addString(LazyLoadingString * stringToAdd, LazyLoadingString ** arrayOfManagedLazyStringPointers, uint8_t size) {
    // Calculate hash to determine placement in the array
    uint8_t placement = getHash(stringToAdd, size);
    for (int i = 0; i < size; i++) {
        placement = (placement + i) % size;
        // Find an empty slot and add the string, start a next fit placement strategy if the hashed placed is already in use
        if (arrayOfManagedLazyStringPointers[placement] == NULL) {
            arrayOfManagedLazyStringPointers[placement] = stringToAdd;
            return &arrayOfManagedLazyStringPointers[placement];
        }
    }
    return NULL;
}

// Retrieve a string from RAM, loading it from flash if not present
char * getString(LazyLoadingString * stringToFetch, StringStorage * stringStorage) {
    if (stringStorage == NULL || stringToFetch == NULL) { return NULL; }

    if (stringToFetch->pointerToString == NULL) {
        stringToFetch->pointerToString = stringStorage->loadStringFromFlash(stringToFetch->flashString);
    }
    return stringToFetch->pointerToString;
}

// Free memory of a single string while retaining management for lazy loading
LazyLoadingString * freeString(LazyLoadingString * stringToKill) {
    free(stringToKill->pointerToString);
    stringToKill->pointerToString = NULL;
    return stringToKill;
}

// Remove string from the array of managed strings, ensuring memory is freed
LazyLoadingString * removeStringFromManagement(LazyLoadingString * stringToKill, LazyLoadingString ** arrayOfManagedLazyStringPointers, uint8_t size) {

    int16_t index = findStringInDb(stringToKill, arrayOfManagedLazyStringPointers, size);
    if (index >= 0) {
        freeString(stringToKill);
        arrayOfManagedLazyStringPointers[index] = NULL;
        return stringToKill;
    }
    return NULL;
}

// Free memory based on a percentage, ensuring at least one element is freed
void freeMemoryRandom(uint8_t percentage, LazyLoadingString ** arrayOfManagedLazyStringPointers, uint8_t size) {
    uint8_t step = 100 / percentage;
    //we delete at least one element
    if (step > (size - 1)) {
        step = size - 1;
    }
    for (uint8_t i = 0; i < size; i += step) {
        freeString(arrayOfManagedLazyStringPointers[i]);
    }
}

uint8_t getHash(LazyLoadingString * stringToAdd, uint8_t size) {
    // hashing with the pointer address, in avr-gcc pointer are 2 byte
    return (((uint16_t) stringToAdd) % size);
}

int16_t findStringInDb(LazyLoadingString * stringToFetch, LazyLoadingString ** arrayOfManagedLazyStringPointers, uint8_t size) {
    if (stringToFetch == NULL) { return -1; }

    uint8_t placement = getHash(stringToFetch, size);
    for (uint8_t i = 0; i < MAX_SIZE_STRING_DB; i++) {
        placement = (placement + i) % MAX_SIZE_STRING_DB;

        if (arrayOfManagedLazyStringPointers[placement] == stringToFetch) { return placement; }
    }
    return -1;
}

// Initialize the StringRepository instance
StringRepository * dOS_initStringRepository(uint8_t size){
    StringRepository * repository = malloc(sizeof(StringRepository));
    if (repository == NULL) { return NULL; }
    else {
        // Assign function pointers
        repository->getString = getString;
        repository->addString = addString;
        repository->freeString = freeString;
        repository->removeStringFromManagement = removeStringFromManagement;
        repository->freeMemoryRandom = freeMemoryRandom;
        if (size>0) {
            repository->arrayOfManagedLazyStringPointers = malloc(size * sizeof(LazyLoadingString *));
            for (int i = 0; i < size; i++) {
                repository->arrayOfManagedLazyStringPointers[i] = NULL;
            }
        } else {
            repository->arrayOfManagedLazyStringPointers = NULL;
        }

        return repository;
    }
}