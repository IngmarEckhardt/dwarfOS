#include <string_storage.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

char * createStringFromFlash(const char * PROGMEM flashString) {
    char * result = (char *) malloc((strlen_P(flashString) + 1) * sizeof(char));
    if (result == NULL) { return NULL; }

    strcpy_P(result, flashString);
    return result;

}
void loadFromFlash(char * stringBuffer, const char * PROGMEM flashString) {
    strcpy_P(stringBuffer, flashString);
}

uint8_t readProgMemByte(const uint8_t * addressOfByte) {
    return pgm_read_byte(addressOfByte);
}

const char initMsgOnFlash[] PROGMEM = " setup complete.\n";
LazyLoadingString initMsg = {.flashString = initMsgOnFlash, .pointerToString = NULL};


StringStorage * dOS_initStringStorage(void) {
    StringStorage * storage = malloc(sizeof(StringStorage));
    if (storage == NULL) { return NULL; }
    else {
        storage->initMsg = initMsg;
        storage->createStringFromFlash = createStringFromFlash;
        storage->loadFromFlash = loadFromFlash;
        storage->readProgMemByte = readProgMemByte;
        return storage;
    }
}