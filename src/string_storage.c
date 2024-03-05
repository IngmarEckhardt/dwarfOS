#include <string_storage.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

char * loadStringFromFlash(const char * PROGMEM flashString) {
    char * result = (char *) malloc((strlen_P(flashString) + 1) * sizeof(char));
    if (result == NULL) { return NULL; }

    strcpy_P(result, flashString);
    return result;
}

const char initMsgOnFlash[] PROGMEM = " setup complete.";
LazyLoadingString initMsg = {.flashString = initMsgOnFlash, .pointerToString = NULL};


StringStorage * dOS_initStringStorage(void) {
    StringStorage * storage = malloc(sizeof(StringStorage));
    if (storage == NULL) { return NULL; }
    else {
        storage->initMsg = initMsg;
        storage->loadStringFromFlash = loadStringFromFlash;
        return storage;
    }
}

LazyLoadingString ** initManagedLazyLoadingStringArray(const char * const arrayWithFlashStrings[], uint8_t amountOfFlashStrings) {

    LazyLoadingString ** managedLazyLoadingStringArray = malloc(amountOfFlashStrings * sizeof(LazyLoadingString *));
    if (managedLazyLoadingStringArray == NULL) { return NULL; }

    for (int i = 0; i < amountOfFlashStrings; i++) {
        LazyLoadingString * stringToAdd = malloc(sizeof(LazyLoadingString));

        if (stringToAdd == NULL) {
            for (int j = 0; j < i; ++j) {
                free(managedLazyLoadingStringArray[j]);
            }
            free(managedLazyLoadingStringArray);
            return NULL;
        }

        stringToAdd->flashString  = arrayWithFlashStrings[i];
        stringToAdd->pointerToString = NULL;
        managedLazyLoadingStringArray[i] = stringToAdd;
    }

    return managedLazyLoadingStringArray;
}