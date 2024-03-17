#include "flash_helper.h"
#include <avr/pgmspace.h>
#include <stdlib.h>
#include "version.h"


char * createStringFromFlash(const char * flashString) {
    char * result = (char *) malloc((strlen_P(flashString) + 1) * sizeof(char));
    if (result == NULL) { return NULL; }
    strcpy_P(result, flashString);
    return result;
}



void loadNearStringFromFlash(char * stringBuffer, const char * flashString) {
    strcpy_P(stringBuffer, flashString);
}

uint8_t readProgMemByte(const uint8_t * addressOfByte) {
    return pgm_read_byte(addressOfByte);
}

int16_t compareWithFlashString(const char * string, const char * flashString) {
    return strcmp_P(string, flashString);
}

uint16_t readNearWord(const uint16_t * intAdress) {
    return pgm_read_word(intAdress);
}

char * loadNearStringFromFile(NearTextFile * textFile, const uint8_t index) {
    size_t entrySize = textFile->indexAmount * sizeof(uint8_t) + textFile->maxLengthOfStrings * sizeof(char);
    for (uint8_t i = 0; i < textFile->amountOfEntries; i++) {

        uint8_t * pointerToIndexArray = (uint8_t *) &textFile + i * entrySize;
        char * pointerToString = (char *) ((uint8_t *) &textFile + i * entrySize +
                                           textFile->indexAmount * sizeof(uint8_t));

        for (uint8_t j = 0; j < textFile->indexAmount; j++) { if (index == pgm_read_byte(pointerToIndexArray + j)) {
                char * stringToReturn = (char *) malloc(textFile->maxLengthOfStrings);
                strcpy_P(stringToReturn, pointerToString);
                return stringToReturn;
            }
        }
    }
    return NULL;
}


#ifdef __AVR_HAVE_ELPM__
char * createFarStringFromFlash(uint_farptr_t farFlashString) {

    char * result = (char *) malloc((strlen_PF(farFlashString) + 1) * sizeof(char));
    if (result == NULL) { return NULL; }
    strcpy_PF(result, farFlashString);
    return result;
}

char * loadFarStringFromFile(FarTextFile * textFile, const uint8_t index) {
    size_t entrySize = textFile->sizeOfIndexArray * sizeof(uint8_t) + textFile->maxLengthOfStrings * sizeof(char);

    for (uint8_t i = 0; i < textFile->amountOfEntries; i++) {
        uint32_t entryAddress = textFile->farPointer + i * entrySize;
        uint32_t numbers = entryAddress;
        uint32_t string = entryAddress + textFile->sizeOfIndexArray * sizeof(uint8_t);
        for (uint8_t j = 0; j < textFile->sizeOfIndexArray; j++) {
            if (index == pgm_read_byte_far(numbers + j)) {
                char * stringToReturn = (char *) malloc(textFile->maxLengthOfStrings);
                strcpy_PF(stringToReturn, string);
                return stringToReturn;
            }
        }
    }
    return NULL;
}


void loadFarStringFromFlash(char * stringBuffer, uint_farptr_t farFlashString) {
    strcpy_PF(stringBuffer, farFlashString);
}

uint16_t readFarWord(uint_farptr_t intAdress) {
    return pgm_read_word_far(intAdress);
}

#endif

const __attribute__((__progmem__)) char initMsgOnFlash[] = DWARFOS_IDENTSTRING;

FlashHelper * dOS_initFlashHelper(void) {
    FlashHelper * helper = malloc(sizeof(FlashHelper));
    if (helper == NULL) { return NULL; }
    else {
        helper->createStringFromFlash = createStringFromFlash;
        helper->readNearWord = readNearWord;
        helper->compareWithFlashString = compareWithFlashString;
        helper->loadNearStringFromFlash = loadNearStringFromFlash;
        helper->loadNearStringFromFile = loadNearStringFromFile;
#ifdef __AVR_HAVE_ELPM__
        helper->createFarStringFromFlash = createFarStringFromFlash;
        helper->loadFarStringFromFlash = loadFarStringFromFlash;
        helper->loadFarStringFromFile = loadFarStringFromFile;
        helper->initMsg = pgm_get_far_address(initMsgOnFlash);
#else
        helper->initMsg = initMsgOnFlash;
#endif
        return helper;
    }
}