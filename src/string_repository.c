#include <string_repository.h>
#include <stdlib.h>


uint8_t getHash(LazyLoadingString* stringToAdd);

int8_t findStringInDb(LazyLoadingString* stringToFetch);

StringRepository * stringRepository;


// add a string to database of managed string to allow automatically lazy loading and make them available to
// the freeMemoryRandom function. Of course, you can do this also by yourself and using just the datastructures.
LazyLoadingString** addString(LazyLoadingString* stringToAdd) {

	uint8_t placement = getHash(stringToAdd);
	for (int i = 0; i < MAX_SIZE_STRING_DB; i++) {
		placement = (placement + i) % MAX_SIZE_STRING_DB;
		if (stringRepository->arrayOfManagedLazyStringPointers[placement] == NULL) {
            stringRepository->arrayOfManagedLazyStringPointers[placement] = stringToAdd;
			return &stringRepository->arrayOfManagedLazyStringPointers[placement];
		}
	}
	return NULL;
}

// return the string from the ram, will load copy it from flash into ram if it's not present there
char* getString(LazyLoadingString * stringToFetch, StringStorage * stringStorage) {
    if (stringStorage == NULL || stringToFetch == NULL ) {
        return NULL;
    }

    if (stringToFetch->pointerToString == NULL) {
		stringToFetch->pointerToString = stringStorage->loadStringFromFlash(stringToFetch->flashString);
	}
	return stringToFetch->pointerToString;
}

//free Memory of a single string, but will keep it in the management and allow lazy loading from flash again if needed
LazyLoadingString* freeString(LazyLoadingString* stringToKill) {
	free(stringToKill->pointerToString);
	stringToKill->pointerToString = NULL;
	return stringToKill;
}

//remove Strings from the array of managed strings with this function to be safe that memory is freed
LazyLoadingString* removeStringFromManagement(LazyLoadingString* stringToKill) {

	int8_t index = findStringInDb(stringToKill);
	if (index >= 0) {
		freeString(stringToKill);
        stringRepository->arrayOfManagedLazyStringPointers[index] = NULL;
		return stringToKill;
	}
	return NULL;
}

//it will round up the percentage until it can free at least one element
void freeMemoryRandom(uint8_t percentage) {
	uint8_t step = 100 / percentage;
	//we delete at least one element
	if (step > (MAX_SIZE_STRING_DB - 1)) {
		step = MAX_SIZE_STRING_DB - 1;
	}
	for (int i = 0; i < MAX_SIZE_STRING_DB; i += step) {
		freeString(stringRepository->arrayOfManagedLazyStringPointers[i]);
	}
}

StringRepository * dOS_initStringRepository(void) {
    StringRepository * repository = malloc(sizeof(StringRepository));
    if (repository == NULL) {
        return NULL;
    } else {
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

//Hashing will only work until 255 managed strings
uint8_t getHash(LazyLoadingString * stringToAdd) {
	return (((uint16_t) stringToAdd) % MAX_SIZE_STRING_DB);
}

int8_t findStringInDb(LazyLoadingString* stringToFetch) {
	uint8_t placement = getHash(stringToFetch);
	for (int i = 0; i < MAX_SIZE_STRING_DB; i++) {
		placement = (placement + i) % MAX_SIZE_STRING_DB;

		if (stringRepository->arrayOfManagedLazyStringPointers[placement] == stringToFetch) {
			return (int8_t) placement;
		}
	}
	return -1;
}