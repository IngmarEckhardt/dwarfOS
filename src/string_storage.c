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