#ifndef DWARFOS_STRING_STORAGE_H
#define DWARFOS_STRING_STORAGE_H

#include "string_repository.h"
#include <avr/pgmspace.h>

LazyLoadingString initMsg;

char* loadStringFromFlash(const char* PROGMEM flashString);

#endif //DWARFOS_STRING_STORAGE_H