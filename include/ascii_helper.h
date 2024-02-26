#ifndef DWARFOS_ASCII_HELPER_H
#define DWARFOS_ASCII_HELPER_H

#include "stdint.h"


typedef struct {
    void (* integerToAscii)(char * result, uint32_t num, uint8_t size, uint8_t position);
    char * (* concatStrings)(uint8_t amountOfStrings, char* strings[]);
} AsciiHelper;

AsciiHelper * dOS_initAsciiHelper(void);


#endif //DWARFOS_ASCII_HELPER_H
