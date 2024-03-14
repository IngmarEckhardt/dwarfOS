#include <flash_helper.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

char * createStringFromFlash(const char  *  flashString) {
    char * result = (char *) malloc((strlen_P(flashString) + 1) * sizeof(char));
    if (result == NULL) { return NULL; }

    strcpy_P(result, flashString);
    return result;

}

char * createFarStringFromFlash(uint_farptr_t farFlashString) {

    char * result = (char *) malloc((strlen_PF(farFlashString) + 1) * sizeof(char));
    if (result == NULL) { return NULL; }

    strcpy_PF(result, farFlashString);
    return result;

}
void loadStringFromFlash(char * stringBuffer, const char * flashString) {
    strcpy_P(stringBuffer, flashString);
}

void loadFarStringFromFlash(char * stringBuffer, uint_farptr_t farFlashString) {
    strcpy_PF(stringBuffer, farFlashString);
}

uint8_t readProgMemByte(const uint8_t * addressOfByte) {
    return pgm_read_byte(addressOfByte);
}

#ifdef DWARFOS_2560
uint8_t readFarProgMemByte(uint_farptr_t addressOfByte) {
    return pgm_read_byte_far(addressOfByte);
}
#endif

int16_t compareWithFlashString(const char * string, const char * flashString) {
    return strcmp_P(string, flashString);
}
uint16_t readNearWord(const uint16_t * intAdress) {
    return pgm_read_word(intAdress);
}
#ifdef DWARFOS_2560
uint16_t readFarWord(uint_farptr_t intAdress) {
    return pgm_read_word_far(intAdress);
}
#endif
const __attribute__((section(".progmemx.data"))) char initMsgOnFlash[] = " setup complete.\n";

FlashHelper * dOS_initFlashHelper(void) {
    FlashHelper * helper = malloc(sizeof(FlashHelper));
    if (helper == NULL) { return NULL; }
    else {
        helper->initMsg =  pgm_get_far_address(initMsgOnFlash);
        helper->createStringFromFlash = createStringFromFlash;
        helper->createFarStringFromFlash = createFarStringFromFlash;
        helper->loadStringFromFlash = loadStringFromFlash;
        helper->loadFarStringFromFlash = loadFarStringFromFlash;
        helper->readProgMemByte = readProgMemByte;
#ifdef DWARFOS_2560
        helper->readFarProgMemByte = readFarProgMemByte;
#endif
        helper->readNearWord = readNearWord;
        #ifdef DWARFOS_2560
        helper->readFarWord = readFarWord;
#endif
        helper->compareWithFlashString = compareWithFlashString;
        return helper;
    }
}