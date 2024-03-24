#include "dwarf-os/flash_helper.h"
#include <stdlib.h>
#include <string.h>


char* createNearStringFromFlash(const char* flashString) {

    return flashString;
}

char * loadFromFlash(char * stringBuffer, const char * flashString) {
    strcpy(stringBuffer,flashString);
    return stringBuffer;
}

uint8_t readProgMemByte(const uint8_t * addressOfByte) {
    return 0;
}



FlashHelper * dOS_initFlashHelper(uint8_t desiredState) {
    FlashHelper * helper = malloc(sizeof(FlashHelper));
    if (helper == NULL) { return NULL; }
    else {
        helper->createString_P = (char * (*)(uint32_t, FlashHelper *)) createNearStringFromFlash;
        helper->copyString_P = (char * (*)(char *, uint32_t)) loadFromFlash;
        return helper;
    }
}