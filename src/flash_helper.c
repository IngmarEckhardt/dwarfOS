#include <dwarf-os/flash_helper.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stdio.h>
#include <dwarf-os/version.h>
#include <dwarf-os/stdio.h>

int compareStringNear(const char * string, uint32_t flashString) {
    return strcmp_P(string, (const char *) flashString);
}

char * copyStringNear(char * stringBuffer, uint32_t pointerToFlashString) {
    return strcpy_P(stringBuffer, (const char *) pointerToFlashString);
}


uint16_t (* lengthOf)(uint32_t flashString);

uint16_t lengthOfNear(uint32_t flashString) { return strlen_P((const char *) flashString); }


int (* putStringToStdOut)(uint32_t flashString);

int putStringToStdOutNear(uint32_t flashString) { return puts_P((const char *) flashString); }


uint8_t (* readProgMemByte)(uint32_t address);

uint8_t readProgMemByteNear(uint32_t address) { return pgm_read_byte((uint8_t *) address); }

#ifdef __AVR_HAVE_ELPM__

uint8_t readProgMemByteFar(uint32_t address) { return pgm_read_byte_far(address); }

#endif
#define isEqual(index, pointerToByte) index == readProgMemByte(pointerToByte)

uint32_t findStringInFile(TextFile * textFile, const uint8_t index) {
    size_t entrySize = textFile->sizeOfIndexArray * sizeof(uint8_t) + textFile->maxLengthOfStrings * sizeof(char);
    //without an array with numbers, we just return the string that is found at the array index position
    if (!textFile->sizeOfIndexArray) { return (textFile->pointerToArray + index * entrySize); }
    for (uint8_t i = 0; i < textFile->amountOfEntries; i++) {
        uint32_t pointerToEntry = textFile->pointerToArray + i * entrySize;
        uint32_t pointerToNumbers = pointerToEntry;
        uint32_t pointerToStringInEntry = pointerToEntry + textFile->sizeOfIndexArray * sizeof(uint8_t);
        for (uint8_t j = 0; j < textFile->sizeOfIndexArray; j++) {
            if (isEqual(index, pointerToNumbers)) { return pointerToStringInEntry; }
            else { pointerToNumbers++; }
        }
    }
    return 0;
}

//the pointerToArray has to be set before with pgm get far address or just cast the normal 16 bit pointer into the uint32_t for near pgm
char * createFromFile_P(TextFile * textFile, const uint8_t index, FlashHelper * helper) {
    char * stringToReturn = NULL;
    uint32_t flashString = findStringInFile(textFile, index);

    if (flashString != 0) {
        stringToReturn = (char *) malloc(textFile->maxLengthOfStrings);
        if (stringToReturn) { helper->loadString_P(stringToReturn, flashString); }
    }
    return stringToReturn;
}

//the pointerToArray has to be set before with pgm get far address or just cast the normal 16 bit pointer into the uint32_t for near pgm
int16_t putFileString_P(TextFile * textFile, const uint8_t index) {
    uint32_t flashString = findStringInFile(textFile, index);
    if (flashString != 0) { return putStringToStdOut(flashString); }
    return -1;
}

char * createString_P(uint32_t flashString, FlashHelper * helper) {
    char * result = (char *) malloc((lengthOf(flashString) + 1) * sizeof(char));
    if (result == NULL) { return NULL; }
    helper->loadString_P(result, flashString);
    return result;
}

// need an implementation, returns true for safety until then, should measure if the user on a ELPM Device uses the pgm beyond 64kB
uint8_t farProgmemIsUsed(void) { return 1; }

const __attribute__((__progmem__)) char initMsgOnFlash[] = DWARFOS_IDENTSTRING;

// desiredState for ELPM devices 0, we measure if 64kB Flash is used, 1 use near pgm getter, 2 use far pgm getters
FlashHelper * dOS_initFlashHelper(uint8_t desiredState) {
    FlashHelper * helper = malloc(sizeof(FlashHelper));
    if (helper == NULL) { return NULL; }
    else {
        helper->createString_P = createString_P;
        helper->createFromFile_P = createFromFile_P;
        helper->putFileString_P = putFileString_P;

        helper->compareString_P = compareStringNear;
        helper->loadString_P = copyStringNear;
        lengthOf = lengthOfNear;
        putStringToStdOut = putStringToStdOutNear;
        readProgMemByte = readProgMemByteNear;
        helper->initMsg = (uint32_t) &initMsgOnFlash;

#ifdef __AVR_HAVE_ELPM__
        if (!desiredState) { desiredState = farProgmemIsUsed() + 1; }
        if (desiredState == 2) {
            helper->compareString_P =  strcmp_PF;
            lengthOf = strlen_PF;
            putStringToStdOut = puts_PF;
            readProgMemByte = readProgMemByteFar;
            helper->loadString_P = strcpy_PF;
        }
        helper->initMsg = pgm_get_far_address(initMsgOnFlash);
#endif
        return helper;
    }
}