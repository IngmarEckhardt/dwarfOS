#ifndef DWARFOS_ASCII_HELPER_H
#define DWARFOS_ASCII_HELPER_H
#include "stdint.h"

void integerToAscii(volatile char* result, uint32_t num, uint8_t size, uint8_t position);
#endif //DWARFOS_ASCII_HELPER_H
