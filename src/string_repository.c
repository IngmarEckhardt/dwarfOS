#include <string_repository.h>
#include <string_storage.h>
#include <stdlib.h>


uint8_t getHash(LazyLoadingString* stringToAdd);

int8_t findStringInDb(LazyLoadingString* stringToFetch);

LazyLoadingString* arrayOfManagedLazyStringPointers[MAX_SIZE_STRING_DB] = {};


// add a string to database of managed string to allow automatically lazy loading and make them available to
// the freeMemoryRandom function. Of course, you can do this also by yourself and using just the datastructures.
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

// return the string from the ram, will load copy it from flash into ram if it's not present there
char* getString(LazyLoadingString* stringToFetch) {
	if (stringToFetch->pointerToString == NULL) {
		stringToFetch->pointerToString = loadStringFromFlash(stringToFetch->flashString);
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
		arrayOfManagedLazyStringPointers[index] = NULL;
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
		freeString(arrayOfManagedLazyStringPointers[i]);
	}
}

//Hashing will only work until 255 managed strings
uint8_t getHash(LazyLoadingString* stringToAdd) {
	return (((uint16_t) stringToAdd) % MAX_SIZE_STRING_DB);
}



int8_t findStringInDb(LazyLoadingString* stringToFetch) {
	uint8_t placement = getHash(stringToFetch);
	for (int i = 0; i < MAX_SIZE_STRING_DB; i++) {
		placement = (placement + i) % MAX_SIZE_STRING_DB;

		if (arrayOfManagedLazyStringPointers[placement] == stringToFetch) {
			return (int8_t) placement;
		}
	}
	return -1;
}