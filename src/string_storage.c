#include <avr/pgmspace.h>
#include <string_storage.h>

const char initMsgOnFlash[] PROGMEM = " setup complete ";
LazyLoadingString initMsg = {.flashString = initMsgOnFlash, .pointerToString = NULL};