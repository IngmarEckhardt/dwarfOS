#include <ascii_helper.h>
#include <stdlib.h>
#include <string.h>

uint16_t computeArrayWithStringLengths(uint8_t amountStrings, char * strings[], uint16_t * stringLengthArray);

void integerToAscii(char * result, uint32_t num, uint8_t size, uint8_t position) {
    uint32_t divisor = 1;
    for (uint8_t i = 0; i < size - 1; ++i) {
        divisor *= 10;
    }

    for (uint8_t i = position; i < position + size; ++i) {
        uint8_t digit = num / divisor;
        result[i] = digit + '0';
        num %= divisor;
        divisor /= 10;
    }
}

char * concatStrings(uint8_t amountOfStrings, char * strings[]) {

    uint16_t * stringLengthArray = (uint16_t *) malloc(amountOfStrings * sizeof(uint16_t));
    if (stringLengthArray == NULL) {
        return NULL;
    }
    uint16_t total_length = computeArrayWithStringLengths(amountOfStrings, strings, stringLengthArray);

    char * stringToReturn = malloc((total_length + 1) * sizeof(char));
    if (stringToReturn == NULL) {
        free(stringLengthArray);
        return NULL;
    }
    char * destination = stringToReturn;

    for (uint8_t i = 0; i < amountOfStrings; i++) {
        memcpy(destination, strings[i], stringLengthArray[i]);
        destination += stringLengthArray[i];
    }
    *destination = '\0';
    free(stringLengthArray);
     return stringToReturn;
}


uint16_t computeArrayWithStringLengths(uint8_t amountStrings, char * strings[], uint16_t * stringLengthArray) {
    uint16_t total_length = 0;

    for (uint8_t i = 0; i < amountStrings; i++) {
        uint16_t len = 0;
        char * temp = strings[i];  // Temporary pointer to iterate over the string
        while (*temp != '\0') {
            len++;
            temp++;
        }
        total_length += len;
        stringLengthArray[i] = len;
    }
    return total_length;
}

AsciiHelper * dOS_initAsciiHelper(void) {
    AsciiHelper * helper = malloc(sizeof(AsciiHelper));
    if (helper == NULL) {
        return NULL;
    } else {
        helper->integerToAscii = integerToAscii;
        helper->concatStrings = concatStrings;
        return helper;
    }
}
