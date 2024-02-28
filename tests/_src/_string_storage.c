#include "../_include/_string_storage.h"
#include <stdlib.h>


char* loadStringFromFlash(const char* flashString) {

    return "mock";
}

const char initMsgOnFlash[] = " setup complete.";
LazyLoadingString initMsg = {.flashString = initMsgOnFlash, .pointerToString = NULL};

StringStorage * dOS_initStringStorage(void) {
    StringStorage * storage = malloc(sizeof(StringStorage));
    if (storage == NULL) {
        return NULL;
    } else {
        storage->initMsg=initMsg;
        storage->loadStringFromFlash=loadStringFromFlash;
        return storage;
    }
}