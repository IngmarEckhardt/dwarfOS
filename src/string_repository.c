#include <string_repository.h>
#include <stdlib.h>

// Hashing function (limited to 255 managed strings)
uint8_t getHash(LazyLoadingString * stringToAdd);

// Find a string in the managed database, has to be int16_t to return -1 as fail, and hold all positive values for uint8
int16_t findStringInDb(LazyLoadingString * stringToFetch);

// Global pointer to the StringRepository instance
StringRepository * stringRepository;


// Add a string to the database of managed strings, enabling lazy loading and availability for freeMemoryRandom function.
// Alternatively, users can manage this manually using the data structures.
// uses placement with hashing, best case O(1), could be changed to placement without hashing for small amount of strings
LazyLoadingString ** addString(LazyLoadingString * stringToAdd) {
    // Calculate hash to determine placement in the array
    uint8_t placement = getHash(stringToAdd);
    for (int i = 0; i < MAX_SIZE_STRING_DB; i++) {
        placement = (placement + i) % MAX_SIZE_STRING_DB;
        // Find an empty slot and add the string, start a next fit placement strategy if the hashed placed is already in use
        if (stringRepository->arrayOfManagedLazyStringPointers[placement] == NULL) {
            stringRepository->arrayOfManagedLazyStringPointers[placement] = stringToAdd;
            return &stringRepository->arrayOfManagedLazyStringPointers[placement];
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
LazyLoadingString * removeStringFromManagement(LazyLoadingString * stringToKill) {

    int16_t index = findStringInDb(stringToKill);
    if (index >= 0) {
        freeString(stringToKill);
        stringRepository->arrayOfManagedLazyStringPointers[index] = NULL;
        return stringToKill;
    }
    return NULL;
}

// Free memory based on a percentage, ensuring at least one element is freed
void freeMemoryRandom(uint8_t percentage) {
    uint8_t step = 100 / percentage;
    //we delete at least one element
    if (step > (MAX_SIZE_STRING_DB - 1)) {
        step = MAX_SIZE_STRING_DB - 1;
    }
    for (uint8_t i = 0; i < MAX_SIZE_STRING_DB; i += step) {
        freeString(stringRepository->arrayOfManagedLazyStringPointers[i]);
    }
}

uint8_t getHash(LazyLoadingString * stringToAdd) {
    // hashing with the pointer address
    return (((uint16_t) stringToAdd) % MAX_SIZE_STRING_DB);
}

int16_t findStringInDb(LazyLoadingString * stringToFetch) {
    if (stringToFetch == NULL) { return -1; }

    uint8_t placement = getHash(stringToFetch);
    for (uint8_t i = 0; i < MAX_SIZE_STRING_DB; i++) {
        placement = (placement + i) % MAX_SIZE_STRING_DB;

        if (stringRepository->arrayOfManagedLazyStringPointers[placement] == stringToFetch) { return placement; }
    }
    return -1;
}

// Initialize the StringRepository instance
StringRepository * dOS_initStringRepository(void) {
    StringRepository * repository = malloc(sizeof(StringRepository));
    if (repository == NULL) { return NULL; }
    else {
        // Assign function pointers
        repository->getString = getString;
        repository->addString = addString;
        repository->freeString = freeString;
        repository->removeStringFromManagement = removeStringFromManagement;
        repository->freeMemoryRandom = freeMemoryRandom;
        for (int i = 0; i < MAX_SIZE_STRING_DB; i++) {
            repository->arrayOfManagedLazyStringPointers[i] = NULL;
        }
        stringRepository = repository;
        return repository;
    }
}