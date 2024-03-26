#include <dwarf-os/flash_helper.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stdio.h>
#include <dwarf-os/version.h>

#ifdef __AVR_HAVE_ELPM__

#include <dwarf-os/stdio.h>

uint8_t readProgMemByteFar(uint32_t address) { return pgm_read_byte_far(address); }

// need an implementation, returns true for safety until then, should measure if the user on a ELPM Device uses the pgm beyond 64kB
static uint8_t farProgmemIsUsed(void) { return 1; }

#endif

static char * copyNear(char * strBuf, uint32_t flashStrgPtr) {
    return strcpy_P(strBuf, (const char *) (uintptr_t) flashStrgPtr);
}

static int putStringToStdOutNear(uint32_t flashString) { return puts_P((const char *) (uintptr_t) flashString); }

static int compareNear(const char * string, uint32_t flashStrg) {
    return strcmp_P(string, (const char *) (uintptr_t) flashStrg);
}

static uint16_t lengthOfNear(uint32_t flashString) { return strlen_P((const char *) (uintptr_t) flashString); }

static uint8_t readProgMemByteNear(uint32_t address) { return pgm_read_byte((uint8_t *) (uintptr_t) address); }

#define isEqual(index, pointerToByte) index == helper->readByte_P(pointerToByte)

#define MAXLENGTH_DOS_STRINGS 30
const __attribute__((__progmem__)) char dOS_messages[3][MAXLENGTH_DOS_STRINGS + 1] = {
        DWARFOS_IDENTSTRING,
        ": free Memory is (byte): ",
        "FATAL ERROR! Code: ",
};

#define MAXLENGTH__SHORT_DOS_STRINGS 8
const __attribute__((__progmem__)) char dOS_shortMessages[1][MAXLENGTH__SHORT_DOS_STRINGS + 1] = {
        "%s:%s%d\n",
};

uint32_t findStringInFile(TextFile * textFile, const uint8_t index, FlashHelper * helper) {
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
    uint32_t flashString = findStringInFile(textFile, index, helper);
    if (flashString != 0) {
        stringToReturn = (char *) malloc(textFile->maxLengthOfStrings);
        if (stringToReturn) { helper->copyString_P(stringToReturn, flashString); }
    }
    return stringToReturn;
}

//the pointerToArray has to be set before with pgm get far address or just cast the normal 16 bit pointer into the uint32_t for near pgm
int16_t putFileString_P(TextFile * textFile, const uint8_t index, FlashHelper * helper) {
    uint32_t flashString = findStringInFile(textFile, index, helper);
    if (flashString != 0) { return helper->putString_P(flashString); }
    return -1;
}

char * createString_P(uint32_t flashString, FlashHelper * helper) {
    char * result = NULL;
    if (helper == NULL ||
        (result = (char *) malloc((helper->lengthOfString_P(flashString) + 1) * sizeof(char))) == NULL) {
        return NULL;
    }
    return helper->copyString_P(result, flashString);
}

//if desired String in Ram is 1 the function returns the string, otherwise it puts the string into stdout
char * getOrPutDosMessage(uint8_t message, uint8_t desiredStringInRam, FlashHelper * helper) {
    char * stringToReturn = NULL;
    switch (message) {
        case IDENT_STRING: {
            if (desiredStringInRam) {
                stringToReturn = (char *) malloc(helper->lengthOfString_P(addressOf(dOS_messages[0])) + 1);
                if (stringToReturn) { helper->copyString_P(stringToReturn, addressOf(dOS_messages[0])); }
            } else { helper->putString_P(addressOf(dOS_messages[0])); }
            break;
        }
        case FREE_MEMORY_STRING: {
            if (desiredStringInRam) {
                stringToReturn = (char *) malloc(helper->lengthOfString_P(addressOf(dOS_messages[1])) + 1);
                if (stringToReturn) { helper->copyString_P(stringToReturn, addressOf(dOS_messages[1])); }
            } else { helper->putString_P(addressOf(dOS_messages[1])); }
            break;
        }
        case FATAL_ERROR_STRING: {
            if (desiredStringInRam) {
                stringToReturn = (char *) malloc(helper->lengthOfString_P(addressOf(dOS_messages[2])) + 1);
                if (stringToReturn) { helper->copyString_P(stringToReturn, addressOf(dOS_messages[2])); }
            } else { helper->putString_P(addressOf(dOS_messages[2])); }
            break;
        }
        case TIMESTAMP_STRING_NUMBER_LF_FORMATSTRING: {
            if (desiredStringInRam) {
                stringToReturn = (char *) malloc(helper->lengthOfString_P(addressOf(dOS_shortMessages[0])) + 1);
                if (stringToReturn) { helper->copyString_P(stringToReturn, addressOf(dOS_shortMessages[0])); }
            } else { helper->putString_P(addressOf(dOS_shortMessages[0])); }
            break;
        }
        default: {
        }
    }
    return stringToReturn;
}

// desiredState for ELPM devices 0, we measure if 64kB Flash is used, 1 use near pgm getter, 2 use far pgm getters
FlashHelper * dOS_initFlashHelper(uint8_t desiredState) {
    FlashHelper * helper = malloc(sizeof(FlashHelper));
    if (helper == NULL) { return NULL; }
    else {
        helper->createString_P = createString_P;
        helper->createFileString_P = createFromFile_P;
        helper->putFileString_P = putFileString_P;

        helper->copyString_P = copyNear;
        helper->putString_P = putStringToStdOutNear;
        helper->compareString_P = compareNear;
        helper->lengthOfString_P = lengthOfNear;
        helper->readByte_P = readProgMemByteNear;

        helper->getOrPutDosMessage = getOrPutDosMessage;

#ifdef __AVR_HAVE_ELPM__
        if (!desiredState) { desiredState = farProgmemIsUsed() + 1; }
        if (desiredState == 2) {
            helper->copyString_P = strcpy_PF;
            helper->putString_P = puts_PF;
            helper->compareString_P = strcmp_PF;
            helper->lengthOfString_P = strlen_PF;
            helper->readByte_P = readProgMemByteFar;
        }
#endif
        return helper;
    }
}