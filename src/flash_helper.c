#include <dwarf-os/flash_helper.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <dwarf-os/version.h>
#include <stdio.h>
#include <dwarf-os/stdio.h>

char * createNearStringFromFlash(const char * flashString) {
    char * result = (char *) malloc((strlen_P(flashString) + 1) * sizeof(char));
    if (result == NULL) { return NULL; }
    strcpy_P(result, flashString);
    return result;
}

void loadNearStringFromFlash(char * stringBuffer, const char * flashString) { strcpy_P(stringBuffer, flashString); }

uint8_t readProgMemByte(const uint8_t * addressOfByte) { return pgm_read_byte(addressOfByte); }

int16_t compareWithNearFlashString(const char * string, const char * flashString) { return strcmp_P(string, flashString); }

uint16_t readNearWord(const uint16_t * intAdress) { return pgm_read_word(intAdress); }

char * findStringInNearFile(NearTextFile * textFile, const uint8_t index) {
    size_t entrySize = textFile->sizeOfIndexArray * sizeof(uint8_t) + textFile->maxLengthOfStrings * sizeof(char);

    for (uint8_t i = 0; i < textFile->amountOfEntries; i++) {
        uint8_t * pointerToIndexArray = (uint8_t *) &textFile + i * entrySize;
        char * pointerToString = (char *) ((uint8_t *) &textFile + i * entrySize +
                                           textFile->sizeOfIndexArray * sizeof(uint8_t));

        for (uint8_t j = 0; j < textFile->sizeOfIndexArray; j++) {
            if (index == pgm_read_byte(pointerToIndexArray + j)) {
                return pointerToString;
            }
        }
    }
    return NULL;
}

char * createNearStringFromFile(NearTextFile * textFile, const uint8_t index) {
    char * nearPgmStringToLoad = findStringInNearFile(textFile, index);
    if (nearPgmStringToLoad != NULL) {
        char * stringToReturn = (char *) malloc(textFile->maxLengthOfStrings);
        strcpy_P(stringToReturn, nearPgmStringToLoad);
        return stringToReturn;
    }
    return NULL;
}

int16_t putFileString_P(NearTextFile * textFile, const uint8_t index) {
    char * nearPgmStringToPut = findStringInNearFile(textFile, index);
    if (nearPgmStringToPut != NULL) {
        return puts_P(nearPgmStringToPut);
    }
    return -1;
}

#ifdef __AVR_HAVE_ELPM__
char * createFarStringFromFlash(uint_farptr_t farFlashString) {

    char * result = (char *) malloc((strlen_PF(farFlashString) + 1) * sizeof(char));
    if (result == NULL) { return NULL; }
    strcpy_PF(result, farFlashString);
    return result;
}

uint32_t findStringInFarFile(FarTextFile * textFile, const uint8_t index) {
    size_t entrySize = textFile->sizeOfIndexArray * sizeof(uint8_t) + textFile->maxLengthOfStrings * sizeof(char);

    for (uint8_t i = 0; i < textFile->amountOfEntries; i++) {
        uint32_t pointerToEntry = textFile->farPointer + i * entrySize;
        uint32_t pointerToNumbers = pointerToEntry;
        uint32_t pointerToStringInEntry = pointerToEntry + textFile->sizeOfIndexArray * sizeof(uint8_t);
        for (uint8_t j = 0; j < textFile->sizeOfIndexArray; j++) {
            if (index == pgm_read_byte_far(pointerToNumbers + j)) {
                return pointerToStringInEntry;
            }
        }
    }
    return 0;
}

char * createFarStringFromFile(FarTextFile * textFile, const uint8_t index) {
    uint32_t pointerToString = findStringInFarFile(textFile, index);
    if (pointerToString != 0) {
        char * stringToReturn = (char *) malloc(textFile->maxLengthOfStrings);
        strcpy_PF(stringToReturn, pointerToString);
        return stringToReturn;
    }
    return NULL;
}
int16_t putFileString_PF(FarTextFile * textFile, const uint8_t index) {
    uint32_t pointerToString = findStringInFarFile(textFile, index);
    if (pointerToString != 0) {
        return puts_PF(pointerToString);
    }
    return -1;
}


void loadFarStringFromFlash(char * stringBuffer, uint_farptr_t farFlashString) {
    strcpy_PF(stringBuffer, farFlashString);
}


#endif

const __attribute__((__progmem__)) char initMsgOnFlash[] = DWARFOS_IDENTSTRING;

FlashHelper * dOS_initFlashHelper(void) {
    FlashHelper * helper = malloc(sizeof(FlashHelper));
    if (helper == NULL) { return NULL; }
    else {
        helper->createString_P = createNearStringFromFlash;
        helper->compareString_P = compareWithNearFlashString;
        helper->loadString_P = loadNearStringFromFlash;
        helper->createFromFile_P = createNearStringFromFile;
        helper->putFileString_P = putFileString_P;
#ifdef __AVR_HAVE_ELPM__
        helper->createString_PF = createFarStringFromFlash;
        helper->loadString_PF = loadFarStringFromFlash;
        helper->createFromFile_PF = createFarStringFromFile;
        helper->putFileString_PF = putFileString_PF;
        helper->initMsg = pgm_get_far_address(initMsgOnFlash);
#else
        helper->initMsg = initMsgOnFlash;
#endif
        return helper;
    }
}