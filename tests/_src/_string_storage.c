#include <string_storage.h>
#include <stdlib.h>
#include <string.h>


char* createStringFromFlash(const char* flashString) {

    return flashString;
}

void loadFromFlash(char * stringBuffer, const char * flashString) {
    strcpy(stringBuffer,flashString);
}

uint8_t readProgMemByte(const uint8_t * addressOfByte) {
    return 0;
}


const char initMsgOnFlash[] = " setup complete.";
LazyLoadingString initMsg = {.flashString = initMsgOnFlash, .pointerToString = NULL};

StringStorage * dOS_initStringStorage(void) {
    StringStorage * storage = malloc(sizeof(StringStorage));
    if (storage == NULL) {
        return NULL;
    } else {
        storage->initMsg=initMsg;
        storage->createStringFromFlash=createStringFromFlash;
        storage->readProgMemByte=readProgMemByte;
        storage->loadFromFlash=loadFromFlash;
        return storage;
    }
}