#include "string_storage.h"
#include <avr/pgmspace.h>

//for testing purposes, strings to short for flash storage
const char cet_on_flash[] PROGMEM = " CET ";
LazyLoadingString cet = {.flashString = cet_on_flash, .pointerToString = NULL};

const char cest_on_flash[] PROGMEM = " CEST ";
LazyLoadingString cest = {.flashString = cest_on_flash, .pointerToString = NULL};


const char catDetect_on_flash[] PROGMEM = " Cat detected ";
LazyLoadingString catDetect = {.flashString = catDetect_on_flash, .pointerToString = NULL};

const char noCatDetect_on_flash[] PROGMEM = " no Cat detected, try again ";
LazyLoadingString noCatDetect = {.flashString = noCatDetect_on_flash, .pointerToString = NULL};

const char initMsg_on_flash[] PROGMEM = " setup complete, entering programming mode ";
LazyLoadingString initMsg = {.flashString = initMsg_on_flash, .pointerToString = NULL};
