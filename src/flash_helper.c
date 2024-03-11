#include <flash_helper.h>
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


FlashHelper * dOS_initFlashHelper(void) {
    FlashHelper * helper = malloc(sizeof(FlashHelper));
    if (helper == NULL) { return NULL; }
    else {
        helper->initMsg = initMsg;
        helper->createStringFromFlash = createStringFromFlash;
        helper->loadFromFlash = loadFromFlash;
        helper->readProgMemByte = readProgMemByte;
        return helper;
    }
}